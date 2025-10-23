#!/usr/bin/env python3
"""
OpenCog Cognitive Fusion Engine - Quick Start Demo

This script demonstrates the basic functionality of the OpenCog cognitive
fusion engine integrated into cortex.cpp.

Prerequisites:
- cortex.cpp server running on localhost:39281
- Python 3.7+ with requests library

Usage:
    python3 opencog_demo.py
"""

import requests
import json
import time
import sys

BASE_URL = "http://localhost:39281"

def check_server():
    """Check if cortex.cpp server is running"""
    try:
        response = requests.get(f"{BASE_URL}/healthz", timeout=2)
        return response.status_code == 200
    except requests.exceptions.RequestException:
        return False

def add_knowledge(concept, strength=0.8, confidence=0.7):
    """Add a concept to the atomspace"""
    try:
        response = requests.post(
            f"{BASE_URL}/v1/cognitive/knowledge",
            json={
                "knowledge": {
                    "concept": concept,
                    "strength": strength,
                    "confidence": confidence
                }
            }
        )
        return response.json()
    except Exception as e:
        print(f"Error adding knowledge: {e}")
        return None

def query_knowledge(pattern):
    """Query the atomspace for matching concepts"""
    try:
        response = requests.post(
            f"{BASE_URL}/v1/cognitive/knowledge/query",
            json={"query": pattern}
        )
        return response.json()
    except Exception as e:
        print(f"Error querying knowledge: {e}")
        return None

def perform_reasoning(operation):
    """Perform cognitive reasoning operation"""
    try:
        response = requests.post(
            f"{BASE_URL}/v1/cognitive/reason",
            json={"operation": operation}
        )
        return response.json()
    except Exception as e:
        print(f"Error performing reasoning: {e}")
        return None

def get_stats():
    """Get atomspace statistics"""
    try:
        response = requests.get(f"{BASE_URL}/v1/cognitive/stats")
        return response.json()
    except Exception as e:
        print(f"Error getting stats: {e}")
        return None

def fuse_engines(engines, query):
    """Fuse results from multiple engines"""
    try:
        response = requests.post(
            f"{BASE_URL}/v1/cognitive/fuse",
            json={
                "engines": engines,
                "query": query
            }
        )
        return response.json()
    except Exception as e:
        print(f"Error fusing engines: {e}")
        return None

def print_section(title):
    """Print a formatted section header"""
    print(f"\n{'='*60}")
    print(f"  {title}")
    print(f"{'='*60}\n")

def main():
    print("""
    ╔══════════════════════════════════════════════════════════╗
    ║     OpenCog Cognitive Fusion Engine - Demo              ║
    ║              Powered by cortex.cpp                       ║
    ╚══════════════════════════════════════════════════════════╝
    """)
    
    # Check server
    print("Checking server connection...")
    if not check_server():
        print("❌ Error: Cortex.cpp server is not running!")
        print("Please start the server with: cortex start")
        sys.exit(1)
    print("✅ Server is running\n")
    
    # Demo 1: Adding Knowledge
    print_section("Demo 1: Building Knowledge Base")
    
    ml_concepts = [
        ("artificial_intelligence", 0.95, 0.90),
        ("machine_learning", 0.92, 0.88),
        ("deep_learning", 0.90, 0.85),
        ("neural_networks", 0.89, 0.87),
        ("natural_language_processing", 0.88, 0.84),
        ("computer_vision", 0.87, 0.83),
        ("reinforcement_learning", 0.85, 0.81)
    ]
    
    print("Adding concepts to atomspace...")
    for concept, strength, confidence in ml_concepts:
        result = add_knowledge(concept, strength, confidence)
        if result:
            print(f"  ✓ Added: {concept}")
            print(f"    Strength: {strength}, Confidence: {confidence}")
    
    time.sleep(0.5)
    
    # Demo 2: Pattern Matching
    print_section("Demo 2: Pattern Matching and Queries")
    
    patterns = ["learning", "neural", "intelligence"]
    
    for pattern in patterns:
        print(f"Searching for pattern: '{pattern}'")
        result = query_knowledge(pattern)
        if result and 'matches' in result:
            matches = result['matches']
            print(f"  Found {len(matches)} matches:")
            for match in matches[:3]:  # Show first 3
                name = match.get('name', 'unknown')
                tv = match.get('truth_value', {})
                strength = tv.get('strength', 0)
                confidence = tv.get('confidence', 0)
                print(f"    • {name}")
                print(f"      Strength: {strength:.2f}, Confidence: {confidence:.2f}")
        print()
    
    # Demo 3: Probabilistic Reasoning
    print_section("Demo 3: Probabilistic Reasoning")
    
    print("Performing inductive reasoning...")
    result = perform_reasoning("induce")
    if result and 'result' in result:
        print("  Induction completed:")
        print(f"    {json.dumps(result['result'], indent=4)}")
    
    time.sleep(0.5)
    
    print("\nPerforming deductive reasoning...")
    result = perform_reasoning("deduce")
    if result and 'result' in result:
        print("  Deduction completed:")
        print(f"    {json.dumps(result['result'], indent=4)}")
    
    # Demo 4: AtomSpace Statistics
    print_section("Demo 4: AtomSpace Statistics")
    
    stats = get_stats()
    if stats:
        print(f"Total Atoms: {stats.get('total_atoms', 0)}")
        hw_config = stats.get('hardware_config', {})
        print(f"Hardware Configuration:")
        print(f"  • GPU Enabled: {hw_config.get('use_gpu', False)}")
        print(f"  • Threads: {hw_config.get('num_threads', 0)}")
        print(f"  • Compute Device: {hw_config.get('compute_device', 'unknown')}")
    
    # Demo 5: Multi-Engine Cognitive Fusion
    print_section("Demo 5: Multi-Engine Cognitive Fusion")
    
    print("Fusing results from multiple engines...")
    print("Engines: llama-cpp, onnx-runtime")
    print("Query: What are the key concepts in machine learning?")
    
    result = fuse_engines(
        ["llama-cpp", "onnx-runtime"],
        "What are the key concepts in machine learning?"
    )
    
    if result:
        print(f"\nFusion Result:")
        print(f"  Operation: {result.get('operation', 'unknown')}")
        print(f"  Confidence: {result.get('confidence', 0):.2f}")
        print(f"  Engine Used: {result.get('engine_used', 'unknown')}")
        print(f"  Result: {json.dumps(result.get('result', {}), indent=4)}")
    
    # Summary
    print_section("Demo Complete!")
    
    final_stats = get_stats()
    if final_stats:
        print(f"Final Knowledge Base Size: {final_stats.get('total_atoms', 0)} atoms")
    
    print("""
    ✨ OpenCog Cognitive Fusion Engine Demo Completed!
    
    Next Steps:
    - Explore the API documentation: /v1/cognitive/*
    - Build your own knowledge graphs
    - Integrate with multiple AI models
    - Implement cognitive reasoning in your applications
    
    For more examples, see: docs/opencog-examples.md
    """)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\nDemo interrupted by user.")
        sys.exit(0)
    except Exception as e:
        print(f"\n❌ Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
