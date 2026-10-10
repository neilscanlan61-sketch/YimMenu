#pragma once
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace big::Animation
{
    constexpr std::uint64_t MaxDecodedBytes = 64ULL * 1024 * 1024;
    constexpr unsigned MaxFrames = 180, MaxDimension = 1024;
    struct Frame { std::vector<std::uint8_t> Pixels; unsigned DelayMs = 100; };
    struct Clip { unsigned Width = 0, Height = 0; std::vector<Frame> Frames; };
    inline void ValidateDimensions(unsigned w, unsigned h, unsigned count)
    {
        if (!w || !h || w > MaxDimension || h > MaxDimension || !count || count > MaxFrames
            || std::uint64_t(w) * h * 4 * count > MaxDecodedBytes)
            throw std::runtime_error("GIF exceeds 1024px, 180 frames or 64 MiB decoded limits.");
    }
    inline std::size_t FrameAt(const Clip& clip, double elapsedMs)
    {
        if (clip.Frames.empty()) return 0;
        std::uint64_t total = 0;
        for (const auto& f : clip.Frames) total += std::clamp(f.DelayMs, 20U, 10000U);
        if (!std::isfinite(elapsedMs) || elapsedMs < 0) elapsedMs = 0;
        double position = std::fmod(elapsedMs, static_cast<double>(total));
        for (std::size_t i = 0; i < clip.Frames.size(); ++i)
        {
            const unsigned delay = std::clamp(clip.Frames[i].DelayMs, 20U, 10000U);
            if (position < delay) return i;
            position -= delay;
        }
        return 0;
    }
    // Composite GIF subrectangles, including transparency and disposal 2/3.
    // Disposal-2 clears to transparent so the menu's base shows through.
    class Canvas
    {
        unsigned m_Width, m_Height;
        std::vector<std::uint8_t> m_Pixels, m_Previous;
        unsigned m_Disposal = 0, m_Left = 0, m_Top = 0, m_RectWidth = 0, m_RectHeight = 0;
    public:
        Canvas(unsigned w, unsigned h) : m_Width(w), m_Height(h)
        {
            ValidateDimensions(w,h,1); m_Pixels.resize(std::size_t(w)*h*4);
        }
        Frame Push(unsigned left, unsigned top, unsigned w, unsigned h,
            const std::vector<std::uint8_t>& rgba, unsigned disposal, unsigned delay)
        {
            if (!w || !h || w > m_Width || h > m_Height || left > m_Width-w || top > m_Height-h
                || rgba.size() != std::size_t(w)*h*4 || disposal > 3)
                throw std::runtime_error("Invalid GIF frame rectangle or disposal.");
            if (m_Disposal == 2)
                for (unsigned y = m_Top; y < m_Top+m_RectHeight; ++y)
                    std::fill_n(m_Pixels.begin()+(std::size_t(y)*m_Width+m_Left)*4,m_RectWidth*4,0);
            else if (m_Disposal == 3 && !m_Previous.empty()) m_Pixels = m_Previous;
            if (disposal == 3) m_Previous = m_Pixels; else m_Previous.clear();
            for (unsigned y = 0; y < h; ++y) for (unsigned x = 0; x < w; ++x)
            {
                const auto src = (std::size_t(y)*w+x)*4;
                const auto dst = (std::size_t(y+top)*m_Width+x+left)*4;
                if (rgba[src+3] == 0) continue;
                // GIF palette alpha is either opaque or transparent.
                std::copy_n(rgba.begin()+src,4,m_Pixels.begin()+dst);
            }
            m_Disposal=disposal; m_Left=left; m_Top=top; m_RectWidth=w; m_RectHeight=h;
            return {m_Pixels,std::clamp(delay,20U,10000U)};
        }
    };
}
