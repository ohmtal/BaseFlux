# BaseFlux - Minimalist SDL3 and ImGui base

Base to build an cross platform Application with SDL3 and ImGui. 

---

# BaseFlux.h : one header only implementation

Only five steps to get it work:

- 1. set your custom settings like Caption, Company .. 
- 2. InitSDL
- 3. InitImGui 
- 4. Setup Render and Event Callback 
- 5. Execute 


---
# Demo with cmake build system:

- fetch SDL3 and staticly link it 
- fetch ImGui and add it as lib

Compile and run : 
```
cd demo
cmake -S . -B build
cmake --build build
./BaseFluxDemo
```

![BaseFlux Demo Screenshot](Screenshot_2026-03-15_08-03-58.png)



