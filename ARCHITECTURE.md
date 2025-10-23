# OpenCog Cognitive Fusion Engine - System Architecture

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         CORTEX.CPP COGNITIVE STACK                          │
└─────────────────────────────────────────────────────────────────────────────┘

                                 ┌───────────────┐
                                 │  HTTP Client  │
                                 │  (REST API)   │
                                 └───────┬───────┘
                                         │
                     ┌───────────────────┼───────────────────┐
                     │                   │                   │
         ┌───────────▼──────────┐ ┌─────▼──────┐ ┌─────────▼─────────┐
         │  Standard Endpoints  │ │  Cognitive │ │  Engine Mgmt      │
         │  /v1/chat/completions│ │  Endpoints │ │  /v1/engines      │
         │  /v1/embeddings      │ │  /v1/cog/* │ │  /v1/models       │
         └──────────┬───────────┘ └─────┬──────┘ └─────────┬─────────┘
                    │                   │                   │
┌───────────────────┴───────────────────┴───────────────────┴─────────────────┐
│                           DROGON HTTP FRAMEWORK                              │
└───────────────────────────────────────┬──────────────────────────────────────┘
                                        │
                    ┌───────────────────┼───────────────────┐
                    │                   │                   │
        ┌───────────▼──────────┐ ┌─────▼────────────┐ ┌───▼────────────┐
        │   Models Controller  │ │  Cognitive Ctrl  │ │ Engines Ctrl   │
        │   (models.cc)        │ │  (cognitive.cc)  │ │ (engines.cc)   │
        └──────────┬───────────┘ └─────┬────────────┘ └───┬────────────┘
                   │                   │                   │
┌──────────────────┴───────────────────┴───────────────────┴──────────────────┐
│                              SERVICE LAYER                                   │
│  ┌────────────────┐ ┌─────────────────┐ ┌──────────────────────────────┐   │
│  │  Model Service │ │ Inference Svc   │ │    Engine Service            │   │
│  │                │ │                 │ │  - LoadEngine()              │   │
│  │  - Load/Unload │ │ - HandleChat    │ │  - UnloadEngine()            │   │
│  │  - List Models │ │ - HandleEmbed   │ │  - GetLoadedEngines()        │   │
│  └────────────────┘ └─────────────────┘ └──────────────────────────────┘   │
└──────────────────────────────────────────────────────────────────────────────┘
                                        │
            ┌───────────────────────────┼───────────────────────────┐
            │                           │                           │
┌───────────▼────────┐    ┌─────────────▼───────────────┐  ┌───────▼──────────┐
│   Local Engine     │    │   OpenCog Engine            │  │  Remote Engine   │
│   (llama.cpp)      │    │   (Cognitive Fusion)        │  │  (API Gateway)   │
│                    │    │                             │  │                  │
│  - Local LLM       │    │  ┌────────────────────────┐ │  │  - OpenAI API    │
│  - GGUF Models     │    │  │   Cognitive Controller │ │  │  - Anthropic     │
│  - Hardware Accel  │    │  │   (opencog_engine.cc)  │ │  │  - Custom APIs   │
│                    │    │  └───────────┬────────────┘ │  │                  │
└────────────────────┘    │              │              │  └──────────────────┘
                          │  ┌───────────▼────────────┐ │
                          │  │      AtomSpace         │ │
                          │  │  (Knowledge Graph)     │ │
                          │  │                        │ │
                          │  │  - Atoms (Nodes/Links) │ │
                          │  │  - Truth Values        │ │
                          │  │  - Thread-Safe Ops     │ │
                          │  └───────────┬────────────┘ │
                          │              │              │
                          │  ┌───────────▼────────────┐ │
                          │  │   Pattern Matcher      │ │
                          │  │  - Name Patterns       │ │
                          │  │  - Type Filters        │ │
                          │  │  - Graph Queries       │ │
                          │  └───────────┬────────────┘ │
                          │              │              │
                          │  ┌───────────▼────────────┐ │
                          │  │  Cognitive Fusion      │ │
                          │  │  - Multi-Engine Merge  │ │
                          │  │  - Confidence Weighting│ │
                          │  │  - Knowledge Combine   │ │
                          │  └───────────┬────────────┘ │
                          │              │              │
                          │  ┌───────────▼────────────┐ │
                          │  │         PLN            │ │
                          │  │  - Deduction           │ │
                          │  │  - Induction           │ │
                          │  │  - Abduction           │ │
                          │  └───────────┬────────────┘ │
                          │              │              │
                          │  ┌───────────▼────────────┐ │
                          │  │  Hardware Optimizer    │ │
                          │  │  - CPU/GPU Detection   │ │
                          │  │  - Thread Pool Config  │ │
                          │  │  - CUDA/OpenCL/Vulkan  │ │
                          │  └────────────────────────┘ │
                          └─────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                           SHARED INFRASTRUCTURE                              │
│  ┌──────────────┐ ┌──────────────┐ ┌────────────┐ ┌──────────────────────┐ │
│  │  Task Queue  │ │   Database   │ │  Hardware  │ │  Dylib Path Manager  │ │
│  │  (async ops) │ │   (SQLite)   │ │   Service  │ │   (engine loading)   │ │
│  └──────────────┘ └──────────────┘ └────────────┘ └──────────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                              HARDWARE LAYER                                  │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────────────┐ │
│  │   CPU    │ │  NVIDIA  │ │   AMD    │ │  Intel   │ │     Apple        │ │
│  │  (x86)   │ │  (CUDA)  │ │ (OpenCL) │ │ (Vulkan) │ │  (Metal/MPS)     │ │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘ └──────────────────┘ │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Data Flow Example: Cognitive Reasoning Request

```
1. HTTP Request
   │
   ├── POST /v1/cognitive/reason
   │   Body: {"operation": "induce"}
   │
2. Cognitive Controller (cognitive.cc)
   │
   ├── Parse request
   ├── Validate operation
   │
3. OpenCog Engine (opencog_engine.cc)
   │
   ├── Reason() method
   │   │
   │   ├── Get concepts from AtomSpace
   │   │   └── atomspace_->GetAtomsByType(CONCEPT_NODE)
   │   │
   │   ├── Call PLN Induce()
   │   │   └── pln_->Induce(concepts)
   │   │       │
   │   │       ├── Calculate average truth values
   │   │       ├── Create generalized concept
   │   │       └── Add to AtomSpace
   │   │
   │   └── Build JSON response
   │
4. HTTP Response
   └── {
         "operation": "induce",
         "result": {
           "type": 2,
           "name": "induced_concept",
           "truth_value": {
             "strength": 0.85,
             "confidence": 0.80
           }
         }
       }
```

## Multi-Engine Fusion Flow

```
1. Fusion Request
   │
   ├── POST /v1/cognitive/fuse
   │   Body: {
   │     "engines": ["llama-cpp", "onnx-runtime"],
   │     "query": "What is AI?"
   │   }
   │
2. Cognitive Controller
   │
3. OpenCog Engine - FuseEngines()
   │
   ├── For each engine:
   │   │
   │   ├── engine_service_->GetLoadedEngine(name)
   │   ├── engine->HandleChatCompletion(query)
   │   └── Collect CognitiveResult
   │       │
   │       └── {
   │             operation: "inference",
   │             confidence: 0.7,
   │             engine_used: "llama-cpp",
   │             result: {...}
   │           }
   │
   ├── cognitive_fusion_->Fuse(results)
   │   │
   │   ├── Calculate average confidence
   │   ├── Select highest confidence result
   │   └── Return fused result
   │
4. Response with fused knowledge
   └── {
         "operation": "inference",
         "confidence": 0.75,
         "engine_used": "fused",
         "result": {...}
       }
```

## Component Interaction Matrix

```
┌─────────────────┬─────────┬───────────┬───────┬─────┬──────────┬──────────┐
│   Component     │ AtomSpc │ PatternMt │  PLN  │Fusion│ Hardware │ EngineI  │
├─────────────────┼─────────┼───────────┼───────┼─────┼──────────┼──────────┤
│ OpenCog Engine  │   ●●●   │    ●●     │  ●●●  │ ●●● │   ●●●    │   ●●●    │
│ Cognitive Ctrl  │   ●●    │    ●      │   ●   │  ●● │    ●     │    -     │
│ AtomSpace       │   ●●●   │     -     │   -   │  -  │    -     │    -     │
│ Pattern Matcher │   ●●●   │   ●●●     │   -   │  -  │    -     │    -     │
│ PLN             │   ●●●   │    ●      │  ●●● │  -  │    -     │    -     │
│ Cognitive Fusion│   ●●    │    ●      │   ●   │ ●●● │    -     │    -     │
│ Hardware Opt    │    -    │     -     │   -   │  -  │   ●●●    │    -     │
└─────────────────┴─────────┴───────────┴───────┴─────┴──────────┴──────────┘

Legend: ●●● Strong dependency, ●● Moderate use, ● Light use, - No dependency
```

## Thread Safety Model

```
┌───────────────────────────────────────────────────────────────┐
│                  Thread Safety Architecture                    │
└───────────────────────────────────────────────────────────────┘

AtomSpace (atomspace.h)
│
├── mutable std::mutex mutex_
│   │
│   ├── Protected Operations:
│   │   ├── AddAtom()         [Write Lock]
│   │   ├── GetAtom()         [Read Lock]
│   │   ├── GetAtomsByType()  [Read Lock]
│   │   ├── GetAllAtoms()     [Read Lock]
│   │   └── Clear()           [Write Lock]
│   │
│   └── Lock Scope: Method-level (RAII with lock_guard)

OpenCog Engine (opencog_engine.h/cc)
│
├── Components:
│   ├── atomspace_ (thread-safe)
│   ├── pattern_matcher_ (thread-safe via atomspace)
│   ├── cognitive_fusion_ (stateless)
│   └── pln_ (thread-safe via atomspace)
│
└── Concurrent Access: Multiple HTTP requests handled safely

Cognitive Controller (cognitive.cc)
│
└── Request Handling: Each request in separate Drogon thread
    │
    └── Safe because OpenCog Engine components are thread-safe
```

## Memory Management

```
┌─────────────────────────────────────────────────────────────────┐
│                     Memory Architecture                          │
└─────────────────────────────────────────────────────────────────┘

Atom Lifecycle:
│
├── Creation: std::make_shared<Atom>(...)
│   └── Reference counted (shared_ptr)
│
├── Storage: AtomSpace::atoms_
│   └── std::unordered_map<string, shared_ptr<Atom>>
│   └── Key: Type:Name (e.g., "2:machine_learning")
│
├── Deduplication: Automatic via key matching
│   └── Existing atoms reused, truth values merged
│
└── Cleanup: Automatic when shared_ptr ref count -> 0
    └── AtomSpace::Clear() releases all references

Memory Characteristics:
│
├── Efficient: O(1) lookup, O(1) insertion
├── Safe: No manual memory management
├── Scalable: Grows with unique concepts
└── Bounded: Limited by available RAM
```

## Key Design Patterns

1. **EngineI Interface Pattern**
   - OpenCog engine implements standard interface
   - Enables polymorphic engine management
   - Seamless integration with cortex.cpp

2. **Repository Pattern**
   - AtomSpace acts as knowledge repository
   - Centralized knowledge storage
   - Consistent access patterns

3. **Strategy Pattern**
   - Different reasoning strategies (PLN operations)
   - Pluggable cognitive algorithms
   - Easy to extend

4. **Observer Pattern**
   - Event-driven architecture via cortex.cpp
   - Asynchronous operation support
   - Decoupled components

5. **Factory Pattern**
   - Atom creation (Nodes, Links)
   - Type-based instantiation
   - Extensible atom types

6. **Facade Pattern**
   - OpenCog Engine as unified interface
   - Simplifies complex subsystems
   - Clean API for users

## File Organization

```
cortex.cog/
├── engine/
│   ├── extensions/
│   │   ├── opencog-engine/
│   │   │   ├── atom.h                    [Atom types and base classes]
│   │   │   ├── atomspace.h               [Knowledge graph storage]
│   │   │   ├── cognitive_fusion.h        [Multi-engine fusion + PLN]
│   │   │   ├── opencog_engine.h          [Main engine interface]
│   │   │   ├── opencog_engine.cc         [Engine implementation]
│   │   │   ├── CMakeLists.txt            [Build configuration]
│   │   │   └── README.md                 [Engine documentation]
│   │   ├── local-engine/                 [llama.cpp integration]
│   │   └── remote-engine/                [API gateway]
│   ├── controllers/
│   │   ├── cognitive.h                   [Cognitive API interface]
│   │   ├── cognitive.cc                  [Cognitive API impl]
│   │   └── [other controllers...]
│   ├── test/components/
│   │   └── test_opencog_cognitive_fusion.cc  [29 comprehensive tests]
│   └── main.cc                           [Application entry point]
├── docs/
│   └── opencog-examples.md               [Usage examples]
├── opencog_demo.py                       [Interactive demo]
├── IMPLEMENTATION_SUMMARY.md             [Technical overview]
└── README.md                             [Main documentation]
```

This architecture provides a solid foundation for cognitive AI operations in cortex.cpp!
