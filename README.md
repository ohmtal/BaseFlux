# BaseFlux - Minimalist SDL3 and ImGui base

Base to build an cross platform Application or Prototype with SDL3 and ImGui. 

---

# BaseFlux.h : one header only implementation

Only five steps to get it work:

- set your custom settings like Caption, Company .. 
- InitSDL
- InitImGui 
- Setup Render and Event Callback 
- Execute 


---
# Demo with cmake build system:

The first cmake take a while because it:

- fetch SDL3 
- fetch ImGui 

Compile and run : 
```
cd demo
cmake -S . -B build
cmake --build build
./BaseFluxDemo
```

![BaseFlux Demo Screenshot](Screenshot_2026-03-15_08-03-58.png)



