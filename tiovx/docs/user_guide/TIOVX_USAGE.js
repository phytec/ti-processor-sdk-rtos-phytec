var TIOVX_USAGE =
[
    [ "TIOVX Usage Recommendations", "TIOVX_USAGE_REC.html", [
      [ "vxSetNodeTarget()", "TIOVX_USAGE_REC.html#TIOVX_USAGE_REC_SET_NODE_TARGET", null ],
      [ "vxGetRemapPoint() and vxSetRemapPoint()", "TIOVX_USAGE_REC.html#TIOVX_USAGE_REC_REMAP", null ],
      [ "Stride Alignment for Images and Raw Images", "TIOVX_USAGE_REC.html#TIOVX_USAGE_STRIDE", null ],
      [ "Graph Performance API's with pipelining", "TIOVX_USAGE_REC.html#TIOVX_USAGE_REC_GRAPH_PERF", null ],
      [ "Enabling a node within the graph to run at lower FPS than rest of graph", "TIOVX_USAGE_REC.html#TIOVX_USAGE_REC_WORKER_THREAD", null ],
      [ "Virtual Object Support in TIOVX", "TIOVX_USAGE_REC.html#TIOVX_USAGE_VIRTUAL_OBJECTS", null ],
      [ "Map and Copy API Usage in TIOVX", "TIOVX_USAGE_REC.html#TIOVX_USAGE_MAP_COPY_API", null ],
      [ "TIOVX Implementation of Image API's", "TIOVX_USAGE_REC.html#TIOVX_USAGE_IMAGE_API", null ]
    ] ],
    [ "OpenVX Standard Specification Interpretations", "TIOVX_SPEC_INTERPRETATIONS.html", [
      [ "OpenVX 1.1 Specification: Vendor-specific implementation details broken down by section", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_OVX1_1_SPEC", null ],
      [ "TIVX Namespace Naming convention", "TIOVX_SPEC_INTERPRETATIONS.html#TIVX_NAMESPACE", null ],
      [ "Context Creation", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_CONTEXT_CREATION", null ],
      [ "Valid Region", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_VALID_REGION", null ],
      [ "Data Object Mapping", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_OBJ_DATA_MAPPING", null ],
      [ "Tensor Object", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_TENSOR", null ],
      [ "User Data Object", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_USER_DATA_OBJECT", null ],
      [ "Required Node Parameters", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_REQUIRED_PARAMETERS", null ],
      [ "vxRemoveKernel", "TIOVX_SPEC_INTERPRETATIONS.html#VXREMOVEKERNEL", null ],
      [ "vxGetContext", "TIOVX_SPEC_INTERPRETATIONS.html#VXGETCONTEXT", null ],
      [ "Object Array and Object Array Elements", "TIOVX_SPEC_INTERPRETATIONS.html#TIOVX_SPEC_INTERPRETATIONS_CREATE_OBJ_ARR", null ]
    ] ],
    [ "User Kernels and User Target Kernels", "TIOVX_TARGET_KERNEL.html", [
      [ "User Kernels", "TIOVX_TARGET_KERNEL.html#USER_KERNELS", null ],
      [ "User Target Kernels", "TIOVX_TARGET_KERNEL.html#USER_TARGET_KERNELS", [
        [ "Callback Implementation Guidelines", "TIOVX_TARGET_KERNEL.html#USER_TARGET_KERNEL_CALLBACK_GUIDELINES", [
          [ "Standard Callback Implementation Guidelines", "TIOVX_TARGET_KERNEL.html#USER_TARGET_KERNEL_CALLBACK_GUIDELINES_STANDARD", null ],
          [ "Callback Implementation Guidelines with Timeouts", "TIOVX_TARGET_KERNEL.html#USER_TARGET_KERNEL_CALLBACK_GUIDELINES_TIMEOUT", null ]
        ] ],
        [ "Kernel Module Integration Guidelines", "TIOVX_TARGET_KERNEL.html#KERNEL_MODULE_INTEGRATION", null ]
      ] ]
    ] ],
    [ "Adding New Targets to TIOVX", "TIOVX_ADD_TARGET.html", [
      [ "Explanation of Targets in TIOVX", "TIOVX_ADD_TARGET.html#TIOVX_TARGET_EXPLANATION", null ],
      [ "Code Changes to Enable More Targets", "TIOVX_ADD_TARGET.html#TIOVX_TARGET_CODE_CHANGES", [
        [ "Step 1", "TIOVX_ADD_TARGET.html#STEP1", null ],
        [ "Step 2", "TIOVX_ADD_TARGET.html#STEP2", null ],
        [ "Step 3", "TIOVX_ADD_TARGET.html#STEP3", null ],
        [ "Step 4", "TIOVX_ADD_TARGET.html#STEP4", null ],
        [ "Step 5", "TIOVX_ADD_TARGET.html#STEP5", null ],
        [ "Step 6", "TIOVX_ADD_TARGET.html#STEP6", null ]
      ] ],
      [ "Disclaimer about \"priority\" argument for TIOVX targets", "TIOVX_ADD_TARGET.html#TIOVX_TARGET_PRIORITY", null ]
    ] ],
    [ "Target Execution Model", "TIOVX_TARGET_EXECUTION.html", [
      [ "FIFO run to completion per Target", "TIOVX_TARGET_EXECUTION.html#TIOVX_TARGET_EXECUTION_SINGLE", null ],
      [ "Priority-Based Preemption of C7X Targets", "TIOVX_TARGET_EXECUTION.html#TIOVX_TARGET_EXECUTION_MULTI", null ]
    ] ],
    [ "Debug Tools for TIOVX", "TIOVX_DEBUG.html", "TIOVX_DEBUG" ],
    [ "Memory Management in TIOVX", "TIOVX_MEMORY_MANAGEMENT.html", [
      [ "Introductory Concepts", "TIOVX_MEMORY_MANAGEMENT.html#TIOVX_MEMORY_MANAGEMENT_INTRODUCTION", null ],
      [ "TIOVX Create Phase", "TIOVX_MEMORY_MANAGEMENT.html#TIOVX_MEMORY_MANAGEMENT_CREATE", null ],
      [ "TIOVX Verify Phase", "TIOVX_MEMORY_MANAGEMENT.html#TIOVX_MEMORY_MANAGEMENT_VERIFY", null ],
      [ "TIOVX Execute Phase", "TIOVX_MEMORY_MANAGEMENT.html#TIOVX_MEMORY_MANAGEMENT_PROCESS", null ],
      [ "TIOVX Destroy Phase", "TIOVX_MEMORY_MANAGEMENT.html#TIOVX_MEMORY_MANAGEMENT_DELETE", null ],
      [ "TIOVX Memory Optimizations", "TIOVX_MEMORY_MANAGEMENT.html#TIOVX_MEMORY_MANAGEMENT_OPTIMIZATIONS", null ]
    ] ],
    [ "Graph Pipelining in TIOVX", "TIOVX_PIPELINING.html", [
      [ "Node and Graph Parameter Definitions", "TIOVX_PIPELINING.html#NODE_GRAPH_PARAMETER_DEFINITION", null ],
      [ "Performance Considerations of Pipelining", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_PERFORMANCE_OPTIMIZATION", [
        [ "Pipeline depth", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_PERFORMANCE_OPTIMIZATION_PIPE_DEPTH", null ],
        [ "Pipelining with Delay Objects", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_PERFORMANCE_OPTIMIZATION_DELAY_PARAMETERS", null ],
        [ "Buffer depth", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_PERFORMANCE_OPTIMIZATION_BUFFER_DEPTH", null ]
      ] ],
      [ "Pipelining Usage Considerations", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_USAGE_CONSIDERATIONS", [
        [ "Pipelining with Composite Objects", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_USAGE_CONSIDERATIONS_COMPOSITE_OBJECTS", null ],
        [ "Pipelining with Replicate Node", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_USAGE_CONSIDERATIONS_REPLICATE_NODE", null ],
        [ "Graph Parameter Considerations", "TIOVX_PIPELINING.html#TIOVX_PIPELINING_USAGE_CONSIDERATIONS_GRAPH_PARAMETER_SETUP", null ]
      ] ]
    ] ],
    [ "Import Reference and Export Reference Usage in TIOVX", "TIOVX_IMPORT_EXPORT.html", [
      [ "Use Case 1", "TIOVX_IMPORT_EXPORT.html#TIOVX_IMPORT_EXPORT_SCENARIO1", null ],
      [ "Use Case 2", "TIOVX_IMPORT_EXPORT.html#TIOVX_IMPORT_EXPORT_SCENARIO2", null ],
      [ "Use Case 3", "TIOVX_IMPORT_EXPORT.html#TIOVX_IMPORT_EXPORT_SCENARIO3", null ],
      [ "Use Case 4", "TIOVX_IMPORT_EXPORT.html#TIOVX_IMPORT_EXPORT_SCENARIO4", null ],
      [ "Use Case 5", "TIOVX_IMPORT_EXPORT.html#TIOVX_IMPORT_EXPORT_SCENARIO5", null ]
    ] ]
];