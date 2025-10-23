# OpenCog Cognitive Fusion Engine for Cortex.cpp

## Overview

This implementation provides an OpenCog-inspired cognitive architecture as a multi-engine, hardware-optimized cognitive fusion reactor for cortex.cpp. It combines hypergraph knowledge representation, probabilistic reasoning, and multi-engine fusion capabilities.

## Features

### 1. **AtomSpace - Hypergraph Knowledge Representation**
- Stores knowledge as atoms (nodes and links) in a hypergraph structure
- Supports various atom types: ConceptNode, PredicateNode, VariableNode, InheritanceLink, etc.
- Thread-safe concurrent access with automatic truth value merging

### 2. **Probabilistic Logic Networks (PLN)**
- Deduction: Forward reasoning (A→B, B→C ⇒ A→C)
- Induction: Generalization from examples
- Abduction: Finding best explanations for observations
- Truth values with strength and confidence measures

### 3. **Cognitive Fusion**
- Combines results from multiple AI engines
- Confidence-weighted result fusion
- Knowledge integration across multiple atomspaces

### 4. **Pattern Matching**
- Query atoms by name patterns
- Filter by atom types
- Efficient graph traversal algorithms

### 5. **Hardware Optimization**
- Auto-detection of available compute devices (CPU, CUDA, OpenCL, Vulkan)
- Automatic thread pool configuration based on hardware
- GPU acceleration support for cognitive operations
- Memory management and optimization

## API Endpoints

### Knowledge Management

#### Add Knowledge
```bash
POST /v1/cognitive/knowledge
Content-Type: application/json

{
  "knowledge": {
    "concept": "artificial_intelligence",
    "strength": 0.9,
    "confidence": 0.85
  }
}
```

#### Query Knowledge
```bash
POST /v1/cognitive/knowledge/query
Content-Type: application/json

{
  "query": "artificial"
}
```

### Reasoning Operations

#### Perform Reasoning
```bash
POST /v1/cognitive/reason
Content-Type: application/json

{
  "operation": "induce"  // or "deduce", "abduce"
}
```

### Cognitive Fusion

#### Fuse Multiple Engines
```bash
POST /v1/cognitive/fuse
Content-Type: application/json

{
  "engines": ["llama-cpp", "onnx-runtime", "tensorrt"],
  "query": "What is machine learning?"
}
```

### Statistics

#### Get AtomSpace Statistics
```bash
GET /v1/cognitive/stats
```

Response:
```json
{
  "total_atoms": 1234,
  "hardware_config": {
    "use_gpu": true,
    "num_threads": 8,
    "compute_device": "cuda"
  }
}
```

## Integration with Cortex.cpp

The OpenCog engine integrates seamlessly with cortex.cpp's existing infrastructure:

1. **Engine Service Integration**: Registered as a standard cortex.cpp engine
2. **Hardware Service**: Leverages cortex.cpp's hardware detection and optimization
3. **Task Queue**: Uses cortex.cpp's task management for concurrent operations
4. **HTTP API**: Follows OpenAI-compatible API conventions where applicable

## Use Cases

### 1. **Multi-Model Reasoning**
Combine outputs from different AI models with cognitive reasoning to produce more reliable results.

### 2. **Knowledge Graph Construction**
Build and query knowledge graphs from unstructured data using natural language processing.

### 3. **Explainable AI**
Use probabilistic reasoning to provide explanations for AI decisions with confidence measures.

### 4. **Cognitive Robotics**
Enable robots to reason about their environment using hypergraph knowledge representation.

### 5. **Adaptive Learning Systems**
Implement systems that learn and adapt their knowledge base through induction and abduction.

## Architecture

```
┌─────────────────────────────────────────┐
│         Cognitive Controller            │
│    (HTTP API for Cognitive Ops)         │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│         OpenCog Engine                  │
│  ┌─────────────────────────────────┐   │
│  │       AtomSpace                 │   │
│  │  (Hypergraph Knowledge Store)   │   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │   Pattern Matcher               │   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │   Probabilistic Logic Network   │   │
│  │   (Deduction/Induction/Abduction)│   │
│  └─────────────────────────────────┘   │
│  ┌─────────────────────────────────┐   │
│  │   Cognitive Fusion              │   │
│  │   (Multi-Engine Integration)    │   │
│  └─────────────────────────────────┘   │
└─────────────────┬───────────────────────┘
                  │
┌─────────────────▼───────────────────────┐
│      Hardware Optimization Layer        │
│  (CPU/CUDA/OpenCL/Vulkan Detection)     │
└─────────────────────────────────────────┘
```

## Building

The OpenCog engine is built as part of the cortex.cpp build process:

```bash
cd engine
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
make -j4
```

## Example Usage

### Python Client Example

```python
import requests
import json

# Add knowledge
response = requests.post(
    "http://localhost:39281/v1/cognitive/knowledge",
    json={
        "knowledge": {
            "concept": "neural_network",
            "strength": 0.95,
            "confidence": 0.90
        }
    }
)
print(response.json())

# Query knowledge
response = requests.post(
    "http://localhost:39281/v1/cognitive/knowledge/query",
    json={"query": "neural"}
)
print(response.json())

# Perform reasoning
response = requests.post(
    "http://localhost:39281/v1/cognitive/reason",
    json={"operation": "induce"}
)
print(response.json())

# Fuse multiple engines
response = requests.post(
    "http://localhost:39281/v1/cognitive/fuse",
    json={
        "engines": ["llama-cpp", "onnx-runtime"],
        "query": "Explain quantum computing"
    }
)
print(response.json())
```

## Performance Optimization

The engine automatically optimizes for available hardware:

- **CPU**: Multi-threaded processing with thread pool
- **CUDA**: GPU-accelerated graph operations
- **OpenCL**: Cross-platform GPU acceleration
- **Vulkan**: Modern graphics API for compute

Memory usage is optimized through:
- Efficient atom deduplication
- Truth value merging
- Lazy evaluation of pattern matches

## Future Enhancements

- [ ] MOSES (program learning) integration
- [ ] Attention allocation mechanisms
- [ ] Economic attention networks
- [ ] Evolutionary programming for pattern mining
- [ ] Neural-symbolic integration
- [ ] Distributed atomspace for large-scale knowledge graphs
- [ ] Real-time learning and adaptation
- [ ] Integration with vision and speech models

## References

- OpenCog Project: https://opencog.org/
- Cortex.cpp: https://cortex.so/
- Probabilistic Logic Networks: PLN Book
- AtomSpace Design: OpenCog AtomSpace Documentation

## License

This implementation follows the same license as cortex.cpp (see LICENSE file in repository root).
