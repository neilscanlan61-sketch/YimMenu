#include "MiniBackground.hpp"
#include "renderer.hpp"
#include "GifAnimation.hpp"
#include "gui/mini/ClickTheme.hpp"
#include <wincodec.h>
#include <wrl/client.h>
#include <propvarutil.h>
#include <chrono>
#include <limits>
#include <filesystem>

namespace big::mini {
namespace {
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    Animation::Clip clip;
    std::string key,status="No animated GIF loaded.";
    bool reload=true,failed=false;
    std::size_t frame=std::numeric_limits<std::size_t>::max();
    double elapsed=0;
    auto last=std::chrono::steady_clock::now();
        void Check(HRESULT hr, const char* message) { if (FAILED(hr)) throw std::runtime_error(message); }
        struct Apartment
        {
            HRESULT Result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            Apartment() { if (FAILED(Result) && Result != RPC_E_CHANGED_MODE) throw std::runtime_error("Cannot initialize GIF decoder COM apartment."); }
            ~Apartment() { if (SUCCEEDED(Result)) CoUninitialize(); }
        };
        unsigned Metadata(IWICMetadataQueryReader* reader, const wchar_t* name, unsigned fallback)
        {
            PROPVARIANT value{};
            unsigned result = fallback;
            if (reader && SUCCEEDED(reader->GetMetadataByName(name,&value)))
            {
                if (value.vt == VT_UI1) result = value.bVal;
                else if (value.vt == VT_UI2) result = value.uiVal;
                else if (value.vt == VT_UI4) result = value.ulVal;
            }
            PropVariantClear(&value);
            return result;
        }
        Animation::Clip Decode(const std::filesystem::path& path)
        {
            if (!std::filesystem::is_regular_file(path) || std::filesystem::file_size(path)>16ULL*1024*1024)
                throw std::runtime_error("GIF is missing or exceeds 16 MiB compressed limit.");
            Apartment apartment;
            Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
            Check(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,IID_PPV_ARGS(&factory)),"Cannot create WIC decoder.");
            Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
            Check(factory->CreateDecoderFromFilename(path.c_str(),nullptr,GENERIC_READ,WICDecodeMetadataCacheOnDemand,&decoder),"Cannot decode GIF file.");
            GUID container{}; Check(decoder->GetContainerFormat(&container),"Cannot identify GIF container.");
            if (!IsEqualGUID(container,GUID_ContainerFormatGif)) throw std::runtime_error("Background must be a GIF, not just a renamed image.");
            UINT count=0; Check(decoder->GetFrameCount(&count),"Cannot read GIF frames.");
            Microsoft::WRL::ComPtr<IWICMetadataQueryReader> metadata;
            Check(decoder->GetMetadataQueryReader(&metadata),"Cannot read GIF dimensions.");
            Animation::Clip clip;
            clip.Width=Metadata(metadata.Get(),L"/logscrdesc/Width",0);
            clip.Height=Metadata(metadata.Get(),L"/logscrdesc/Height",0);
            Animation::ValidateDimensions(clip.Width,clip.Height,count);
            Animation::Canvas canvas(clip.Width,clip.Height); clip.Frames.reserve(count);
            for (UINT i=0;i<count;++i)
            {
                Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
                Check(decoder->GetFrame(i,&frame),"Cannot read GIF frame.");
                Microsoft::WRL::ComPtr<IWICMetadataQueryReader> frameMetadata;
                Check(frame->GetMetadataQueryReader(&frameMetadata),"Cannot read GIF frame metadata.");
                UINT w=0,h=0; Check(frame->GetSize(&w,&h),"Cannot read GIF frame size.");
                if (!w || !h || w>clip.Width || h>clip.Height) throw std::runtime_error("GIF subframe dimensions are invalid.");
                const unsigned left=Metadata(frameMetadata.Get(),L"/imgdesc/Left",0);
                const unsigned top=Metadata(frameMetadata.Get(),L"/imgdesc/Top",0);
                const unsigned disposal=Metadata(frameMetadata.Get(),L"/grctlext/Disposal",0);
                const unsigned delay=std::clamp(Metadata(frameMetadata.Get(),L"/grctlext/Delay",10),2U,1000U)*10;
                Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
                Check(factory->CreateFormatConverter(&converter),"Cannot create GIF pixel converter.");
                Check(converter->Initialize(frame.Get(),GUID_WICPixelFormat32bppRGBA,WICBitmapDitherTypeNone,nullptr,0,WICBitmapPaletteTypeCustom),"Cannot convert GIF pixels.");
                std::vector<std::uint8_t> rgba(std::size_t(w)*h*4);
                Check(converter->CopyPixels(nullptr,w*4,static_cast<UINT>(rgba.size()),rgba.data()),"Cannot copy GIF frame pixels.");
                clip.Frames.push_back(canvas.Push(left,top,w,h,rgba,disposal,delay));
            }
            return clip;
        }
}
void ResetBackground(){view.Reset();texture.Reset();clip={};key.clear();frame=std::numeric_limits<std::size_t>::max();elapsed=0;last=std::chrono::steady_clock::now();reload=true;failed=false;}
void ReloadBackground(){reload=true;}
const std::string& BackgroundStatus(){return status;}
void TickBackground(bool playing){auto now=std::chrono::steady_clock::now();if(playing)elapsed+=std::chrono::duration<double,std::milli>(now-last).count()*ClickUI::CurrentTheme().BackgroundSpeed;last=now;}
void DrawBackground(ImDrawList* draw,ImVec2 pos,ImVec2 size)
{
    const auto& t=ClickUI::CurrentTheme();
    if(!t.BackgroundEnabled || !draw || size.x<=0 || size.y<=0)return;
    const auto name=ClickUI::ActiveThemeName();
    if(reload || key!=name)
    {
        ResetBackground();reload=false;key=name;
        try{
            auto path=ClickUI::ThemeFolder()/(name+".gif");
            if(!std::filesystem::is_regular_file(path))path=ClickUI::ThemeFolder()/"background.gif";
            clip=Decode(path);status="Loaded "+path.filename().string()+" ("+std::to_string(clip.Frames.size())+" frames).";
        }catch(const std::exception& e){status=e.what();failed=true;return;}
    }
    if(failed || clip.Frames.empty())return;
    try{
        auto* device=g_renderer.mini_device();auto* context=g_renderer.mini_context();if(!device || !context)return;
        if(!texture){
            D3D11_TEXTURE2D_DESC desc{};desc.Width=clip.Width;desc.Height=clip.Height;desc.MipLevels=1;desc.ArraySize=1;
            desc.Format=DXGI_FORMAT_R8G8B8A8_UNORM;desc.SampleDesc.Count=1;desc.Usage=D3D11_USAGE_DEFAULT;desc.BindFlags=D3D11_BIND_SHADER_RESOURCE;
            Check(device->CreateTexture2D(&desc,nullptr,&texture),"Cannot create Legacy GIF texture.");
            Check(device->CreateShaderResourceView(texture.Get(),nullptr,&view),"Cannot create Legacy GIF shader view.");
        }
        const auto next=Animation::FrameAt(clip,elapsed);
        if(next!=frame){context->UpdateSubresource(texture.Get(),0,nullptr,clip.Frames[next].Pixels.data(),clip.Width*4,0);frame=next;}
        ImVec2 uv0{0,0},uv1{1,1};const float aspect=static_cast<float>(clip.Width)/static_cast<float>(clip.Height),target=size.x/size.y;
        if(aspect>target){const float w=target/aspect;uv0.x=(1-w)*.5f;uv1.x=uv0.x+w;}else{const float h=aspect/target;uv0.y=(1-h)*.5f;uv1.y=uv0.y+h;}
        draw->AddImageRounded(reinterpret_cast<ImTextureID>(view.Get()),pos,{pos.x+size.x,pos.y+size.y},uv0,uv1,ImGui::GetColorU32(ImVec4{1,1,1,t.BackgroundOpacity}),t.Rounding);
    }catch(const std::exception& e){status=e.what();failed=true;}
}
}
