# OpenCog Cognitive Fusion Engine Examples

This document provides practical examples of using the OpenCog cognitive fusion engine in cortex.cpp.

## Table of Contents

1. [Basic Knowledge Management](#basic-knowledge-management)
2. [Pattern Matching and Queries](#pattern-matching-and-queries)
3. [Probabilistic Reasoning](#probabilistic-reasoning)
4. [Multi-Engine Cognitive Fusion](#multi-engine-cognitive-fusion)
5. [Building Knowledge Graphs](#building-knowledge-graphs)
6. [Real-World Use Cases](#real-world-use-cases)

## Basic Knowledge Management

### Adding Concepts to AtomSpace

```bash
curl -X POST http://localhost:39281/v1/cognitive/knowledge \
  -H "Content-Type: application/json" \
  -d '{
    "knowledge": {
      "concept": "quantum_computing",
      "strength": 0.9,
      "confidence": 0.85
    }
  }'
```

Response:
```json
{
  "status": "success",
  "atomspace_size": 1
}
```

### Adding Multiple Related Concepts

```python
import requests

concepts = [
    {"concept": "artificial_intelligence", "strength": 0.95, "confidence": 0.90},
    {"concept": "machine_learning", "strength": 0.90, "confidence": 0.92},
    {"concept": "deep_learning", "strength": 0.88, "confidence": 0.85},
    {"concept": "neural_networks", "strength": 0.87, "confidence": 0.88},
    {"concept": "reinforcement_learning", "strength": 0.82, "confidence": 0.80}
]

for concept in concepts:
    response = requests.post(
        "http://localhost:39281/v1/cognitive/knowledge",
        json={"knowledge": concept}
    )
    print(f"Added: {concept['concept']}, Size: {response.json()['atomspace_size']}")
```

## Pattern Matching and Queries

### Querying by Pattern

```bash
curl -X POST http://localhost:39281/v1/cognitive/knowledge/query \
  -H "Content-Type: application/json" \
  -d '{
    "query": "learning"
  }'
```

Response:
```json
{
  "query": "learning",
  "matches": [
    {
      "type": 2,
      "name": "machine_learning",
      "truth_value": {
        "strength": 0.9,
        "confidence": 0.92
      }
    },
    {
      "type": 2,
      "name": "deep_learning",
      "truth_value": {
        "strength": 0.88,
        "confidence": 0.85
      }
    }
  ]
}
```

### Python Query Example

```python
import requests

def query_knowledge(pattern):
    response = requests.post(
        "http://localhost:39281/v1/cognitive/knowledge/query",
        json={"query": pattern}
    )
    return response.json()

# Search for AI-related concepts
results = query_knowledge("intelligence")
print(f"Found {len(results['matches'])} matches:")
for match in results['matches']:
    print(f"  - {match['name']}: strength={match['truth_value']['strength']}")
```

## Probabilistic Reasoning

### Deductive Reasoning

Deduction: If A→B and B→C, then A→C

```bash
# First, add some knowledge
curl -X POST http://localhost:39281/v1/cognitive/knowledge \
  -H "Content-Type: application/json" \
  -d '{
    "knowledge": {
      "concept": "supervised_learning",
      "strength": 0.9,
      "confidence": 0.85
    }
  }'

# Perform deduction
curl -X POST http://localhost:39281/v1/cognitive/reason \
  -H "Content-Type: application/json" \
  -d '{
    "operation": "deduce"
  }'
```

### Inductive Reasoning

Induction: Generalize from examples

```bash
curl -X POST http://localhost:39281/v1/cognitive/reason \
  -H "Content-Type: application/json" \
  -d '{
    "operation": "induce"
  }'
```

### Reasoning Pipeline Example

```python
import requests

class CognitiveReasoner:
    def __init__(self, base_url="http://localhost:39281"):
        self.base_url = base_url
    
    def add_knowledge(self, concept, strength=0.8, confidence=0.7):
        response = requests.post(
            f"{self.base_url}/v1/cognitive/knowledge",
            json={
                "knowledge": {
                    "concept": concept,
                    "strength": strength,
                    "confidence": confidence
                }
            }
        )
        return response.json()
    
    def reason(self, operation):
        response = requests.post(
            f"{self.base_url}/v1/cognitive/reason",
            json={"operation": operation}
        )
        return response.json()
    
    def query(self, pattern):
        response = requests.post(
            f"{self.base_url}/v1/cognitive/knowledge/query",
            json={"query": pattern}
        )
        return response.json()

# Usage
reasoner = CognitiveReasoner()

# Build knowledge base
reasoner.add_knowledge("neural_network", 0.9, 0.85)
reasoner.add_knowledge("backpropagation", 0.88, 0.82)
reasoner.add_knowledge("gradient_descent", 0.87, 0.80)

# Perform induction to find patterns
result = reasoner.reason("induce")
print("Induction result:", result)

# Query for related concepts
matches = reasoner.query("neural")
print(f"Found {len(matches['matches'])} related concepts")
```

## Multi-Engine Cognitive Fusion

### Fusing Results from Multiple Engines

```bash
curl -X POST http://localhost:39281/v1/cognitive/fuse \
  -H "Content-Type: application/json" \
  -d '{
    "engines": ["llama-cpp", "onnx-runtime"],
    "query": "Explain the principles of quantum computing"
  }'
```

### Python Multi-Engine Fusion Example

```python
import requests

def fuse_engines(engines, query):
    response = requests.post(
        "http://localhost:39281/v1/cognitive/fuse",
        json={
            "engines": engines,
            "query": query
        }
    )
    return response.json()

# Combine outputs from multiple AI models
engines = ["llama-cpp", "onnx-runtime", "tensorrt"]
query = "What are the ethical implications of artificial general intelligence?"

result = fuse_engines(engines, query)

print(f"Fused Result:")
print(f"  Operation: {result['operation']}")
print(f"  Confidence: {result['confidence']}")
print(f"  Engine: {result['engine_used']}")
print(f"  Result: {result['result']}")
```

## Building Knowledge Graphs

### Creating a Knowledge Graph for a Domain

```python
import requests
import json

class KnowledgeGraphBuilder:
    def __init__(self, base_url="http://localhost:39281"):
        self.base_url = base_url
        self.concepts_added = 0
    
    def add_concept(self, name, strength=0.8, confidence=0.7):
        response = requests.post(
            f"{self.base_url}/v1/cognitive/knowledge",
            json={
                "knowledge": {
                    "concept": name,
                    "strength": strength,
                    "confidence": confidence
                }
            }
        )
        self.concepts_added += 1
        return response.json()
    
    def get_stats(self):
        response = requests.get(f"{self.base_url}/v1/cognitive/stats")
        return response.json()
    
    def build_ml_knowledge_graph(self):
        """Build a knowledge graph for machine learning domain"""
        
        ml_concepts = {
            # Core concepts
            "machine_learning": (0.95, 0.92),
            "artificial_intelligence": (0.95, 0.90),
            
            # Learning types
            "supervised_learning": (0.90, 0.88),
            "unsupervised_learning": (0.88, 0.85),
            "reinforcement_learning": (0.87, 0.83),
            "semi_supervised_learning": (0.82, 0.78),
            
            # Algorithms
            "neural_networks": (0.93, 0.90),
            "decision_trees": (0.89, 0.87),
            "random_forests": (0.88, 0.86),
            "support_vector_machines": (0.87, 0.85),
            "k_means_clustering": (0.85, 0.82),
            
            # Deep learning
            "deep_learning": (0.92, 0.88),
            "convolutional_neural_networks": (0.90, 0.87),
            "recurrent_neural_networks": (0.89, 0.86),
            "transformers": (0.91, 0.88),
            "attention_mechanism": (0.88, 0.85),
            
            # Optimization
            "gradient_descent": (0.90, 0.88),
            "backpropagation": (0.91, 0.89),
            "adam_optimizer": (0.87, 0.84),
            
            # Applications
            "computer_vision": (0.89, 0.86),
            "natural_language_processing": (0.90, 0.87),
            "speech_recognition": (0.87, 0.84),
            "recommendation_systems": (0.86, 0.83)
        }
        
        print("Building ML Knowledge Graph...")
        for concept, (strength, confidence) in ml_concepts.items():
            self.add_concept(concept, strength, confidence)
            print(f"  Added: {concept}")
        
        stats = self.get_stats()
        print(f"\nKnowledge Graph Stats:")
        print(f"  Total atoms: {stats['total_atoms']}")
        print(f"  Hardware: {stats['hardware_config']['compute_device']}")
        print(f"  Threads: {stats['hardware_config']['num_threads']}")

# Usage
builder = KnowledgeGraphBuilder()
builder.build_ml_knowledge_graph()
```

## Real-World Use Cases

### Use Case 1: Intelligent Document Classification

```python
import requests

class DocumentClassifier:
    def __init__(self):
        self.base_url = "http://localhost:39281"
        self.initialize_knowledge_base()
    
    def initialize_knowledge_base(self):
        """Initialize with document categories"""
        categories = {
            "technical_documentation": 0.9,
            "research_paper": 0.88,
            "business_report": 0.85,
            "legal_document": 0.87,
            "medical_record": 0.86
        }
        
        for category, confidence in categories.items():
            requests.post(
                f"{self.base_url}/v1/cognitive/knowledge",
                json={
                    "knowledge": {
                        "concept": category,
                        "strength": 0.8,
                        "confidence": confidence
                    }
                }
            )
    
    def classify_document(self, document_text):
        """Classify document using cognitive reasoning"""
        
        # Query for matching categories
        response = requests.post(
            f"{self.base_url}/v1/cognitive/knowledge/query",
            json={"query": document_text[:100]}  # Use first 100 chars
        )
        
        matches = response.json().get('matches', [])
        
        if not matches:
            return "unknown", 0.0
        
        # Return category with highest confidence
        best_match = max(matches, 
                        key=lambda x: x['truth_value']['confidence'])
        
        return best_match['name'], best_match['truth_value']['confidence']

# Usage
classifier = DocumentClassifier()
doc = "This research paper presents a novel approach to deep learning..."
category, confidence = classifier.classify_document(doc)
print(f"Category: {category}, Confidence: {confidence}")
```

### Use Case 2: Multi-Model Consensus System

```python
import requests

class ConsensusSystem:
    def __init__(self):
        self.base_url = "http://localhost:39281"
    
    def get_consensus_answer(self, question, models):
        """Get consensus answer from multiple models"""
        
        response = requests.post(
            f"{self.base_url}/v1/cognitive/fuse",
            json={
                "engines": models,
                "query": question
            }
        )
        
        result = response.json()
        
        return {
            "answer": result.get('result', {}).get('response', ''),
            "confidence": result.get('confidence', 0.0),
            "consensus_engine": result.get('engine_used', 'unknown')
        }
    
    def validate_with_knowledge_base(self, answer):
        """Validate answer against knowledge base"""
        
        response = requests.post(
            f"{self.base_url}/v1/cognitive/knowledge/query",
            json={"query": answer[:50]}
        )
        
        matches = response.json().get('matches', [])
        
        if matches:
            avg_confidence = sum(
                m['truth_value']['confidence'] for m in matches
            ) / len(matches)
            return avg_confidence
        
        return 0.0

# Usage
system = ConsensusSystem()

question = "What is the capital of France?"
models = ["llama-cpp", "onnx-runtime"]

result = system.get_consensus_answer(question, models)
validation_score = system.validate_with_knowledge_base(result['answer'])

print(f"Question: {question}")
print(f"Answer: {result['answer']}")
print(f"Model Confidence: {result['confidence']}")
print(f"Validation Score: {validation_score}")
```

### Use Case 3: Adaptive Learning System

```python
import requests
import time

class AdaptiveLearner:
    def __init__(self):
        self.base_url = "http://localhost:39281"
    
    def learn_from_interaction(self, concept, correctness):
        """Update knowledge base based on interaction feedback"""
        
        strength = 0.9 if correctness else 0.5
        confidence = 0.85 if correctness else 0.6
        
        requests.post(
            f"{self.base_url}/v1/cognitive/knowledge",
            json={
                "knowledge": {
                    "concept": concept,
                    "strength": strength,
                    "confidence": confidence
                }
            }
        )
    
    def perform_induction(self):
        """Learn patterns from accumulated knowledge"""
        
        response = requests.post(
            f"{self.base_url}/v1/cognitive/reason",
            json={"operation": "induce"}
        )
        
        return response.json()
    
    def get_learning_progress(self):
        """Get statistics on learning progress"""
        
        response = requests.get(f"{self.base_url}/v1/cognitive/stats")
        return response.json()

# Usage
learner = AdaptiveLearner()

# Simulate learning interactions
interactions = [
    ("python_programming", True),
    ("web_development", True),
    ("machine_learning", True),
    ("quantum_physics", False),
    ("data_structures", True)
]

for concept, correct in interactions:
    learner.learn_from_interaction(concept, correct)
    print(f"Learned: {concept} ({'correct' if correct else 'incorrect'})")
    time.sleep(0.1)

# Induce patterns
patterns = learner.perform_induction()
print(f"\nInduced patterns: {patterns}")

# Check progress
progress = learner.get_learning_progress()
print(f"Total concepts learned: {progress['total_atoms']}")
```

## Getting Statistics and Monitoring

```bash
# Get current atomspace statistics
curl http://localhost:39281/v1/cognitive/stats
```

Response:
```json
{
  "total_atoms": 42,
  "hardware_config": {
    "use_gpu": true,
    "num_threads": 8,
    "compute_device": "cuda"
  }
}
```

## Tips and Best Practices

1. **Truth Values**: Set appropriate strength and confidence values
   - Strength: How likely the statement is true (0.0 - 1.0)
   - Confidence: How confident you are in the strength value (0.0 - 1.0)

2. **Pattern Matching**: Use specific enough patterns to get relevant results

3. **Multi-Engine Fusion**: Combine complementary models for better results
   - Fast model for quick responses
   - Accurate model for critical decisions
   - Specialized models for domain-specific tasks

4. **Knowledge Graph Building**: Start with core concepts and expand gradually

5. **Hardware Optimization**: The engine automatically detects and uses available hardware

## Troubleshooting

### Issue: No matches found
**Solution**: Add more concepts to the knowledge base or use broader patterns

### Issue: Low confidence scores
**Solution**: Add more training examples and perform induction to strengthen patterns

### Issue: Slow responses
**Solution**: Check hardware configuration and consider enabling GPU acceleration

For more information, see the [main README](README.md).
