From the repository root:

```sh
g++ -std=c++20 -Wall -Wextra -Werror -Wno-misleading-indentation -Itests/mini -Itests/mini/mock-include -Isrc tests/mini/main.cpp -o /tmp/legacy-mini-test
/tmp/legacy-mini-test /tmp/legacy-mini-fresh-fixture
```

Uses real MiniUI.cpp, MiniTravel.cpp, view_custom_teleport.cpp, theme parser and GIF policy against a fake game/ImGui. Does not test Windows WIC decode, D3D11 GPU uploads, real JSON disk persistence, or GTA actions. Use a new empty fixture folder for each run.
