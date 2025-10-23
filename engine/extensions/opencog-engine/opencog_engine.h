#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "atomspace.h"
#include "cognitive_fusion.h"
#include "cortex-common/EngineI.h"
#include "json/json.h"
#include "services/engine_service.h"
#include "utils/task_queue.h"

namespace cortex::opencog {

using http_callback = std::function<void(Json::Value&&, Json::Value&&)>;

// Hardware optimization hints
struct HardwareConfig {
  bool use_gpu;
  int num_threads;
  size_t memory_limit_mb;
  std::string compute_device;  // cpu, cuda, opencl, vulkan
  
  HardwareConfig() 
    : use_gpu(false), 
      num_threads(4), 
      memory_limit_mb(1024),
      compute_device("cpu") {}
};

// OpenCog Engine - implements cognitive architecture
class OpenCogEngine : public EngineI {
 public:
  OpenCogEngine(EngineService& engine_service, TaskQueue& q)
      : engine_service_(engine_service), 
        q_(q),
        atomspace_(std::make_shared<AtomSpace>()),
        pattern_matcher_(std::make_shared<PatternMatcher>(atomspace_)),
        cognitive_fusion_(std::make_shared<CognitiveFusion>()),
        pln_(std::make_shared<PLN>(atomspace_)) {
    // Initialize with hardware detection
    DetectAndConfigureHardware();
  }
  
  ~OpenCogEngine() override = default;

  void Load(EngineLoadOption opts) override {
    // Load engine with specified options
    loaded_ = true;
  }

  void Unload(EngineUnloadOption opts) override {
    // Unload engine
    loaded_ = false;
    atomspace_->Clear();
  }

  // Handle chat completion with cognitive reasoning
  void HandleChatCompletion(std::shared_ptr<Json::Value> json_body,
                           http_callback&& callback) override;

  // Handle embedding with knowledge representation
  void HandleEmbedding(std::shared_ptr<Json::Value> json_body,
                      http_callback&& callback) override;

  // Load cognitive model
  void LoadModel(std::shared_ptr<Json::Value> json_body,
                http_callback&& callback) override;

  // Unload cognitive model
  void UnloadModel(std::shared_ptr<Json::Value> json_body,
                  http_callback&& callback) override;

  // Get cognitive model status
  void GetModelStatus(std::shared_ptr<Json::Value> json_body,
                     http_callback&& callback) override;

  // Get list of cognitive models
  void GetModels(std::shared_ptr<Json::Value> jsonBody,
                http_callback&& callback) override;

  bool SetFileLogger(int max_log_lines, const std::string& log_path) override {
    return true;
  }
  
  void SetLogLevel(trantor::Logger::LogLevel logLevel) override {}

  // Stop cognitive inferencing
  void StopInferencing(const std::string& model_id) override {}

  // Cognitive-specific operations
  
  // Add knowledge to atomspace
  void AddKnowledge(std::shared_ptr<Json::Value> json_body,
                   http_callback&& callback);
  
  // Query atomspace
  void QueryKnowledge(std::shared_ptr<Json::Value> json_body,
                     http_callback&& callback);
  
  // Perform reasoning
  void Reason(std::shared_ptr<Json::Value> json_body,
             http_callback&& callback);
  
  // Fuse results from multiple engines
  void FuseEngines(const std::vector<std::string>& engine_names,
                  std::shared_ptr<Json::Value> json_body,
                  http_callback&& callback);

  // Get atomspace statistics
  Json::Value GetAtomSpaceStats() const {
    Json::Value stats;
    stats["total_atoms"] = static_cast<Json::UInt64>(atomspace_->Size());
    stats["hardware_config"]["use_gpu"] = hw_config_.use_gpu;
    stats["hardware_config"]["num_threads"] = hw_config_.num_threads;
    stats["hardware_config"]["compute_device"] = hw_config_.compute_device;
    return stats;
  }

 private:
  void DetectAndConfigureHardware() {
    // Auto-detect available hardware
    // This is a simplified version - real implementation would use
    // actual hardware detection from cortex.cpp
    #ifdef __CUDA_ARCH__
    hw_config_.use_gpu = true;
    hw_config_.compute_device = "cuda";
    #elif defined(__OPENCL_VERSION__)
    hw_config_.use_gpu = true;
    hw_config_.compute_device = "opencl";
    #else
    hw_config_.use_gpu = false;
    hw_config_.compute_device = "cpu";
    #endif
    
    // Set optimal thread count
    hw_config_.num_threads = std::thread::hardware_concurrency();
  }
  
  CognitiveResult ProcessWithCognition(const std::string& input);
  
  EngineService& engine_service_;
  TaskQueue& q_;
  
  // Cognitive components
  std::shared_ptr<AtomSpace> atomspace_;
  std::shared_ptr<PatternMatcher> pattern_matcher_;
  std::shared_ptr<CognitiveFusion> cognitive_fusion_;
  std::shared_ptr<PLN> pln_;
  
  // Hardware configuration
  HardwareConfig hw_config_;
  
  // State
  bool loaded_ = false;
  std::unordered_map<std::string, std::string> loaded_models_;
};

}  // namespace cortex::opencog
