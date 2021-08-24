cdef public __udf_user(s):
    import onnxruntime
    import sys
    import os
    def input_to_tensor(input, onnx_session):
        import torch
        import simpletransformers.model
        import transformers
        def to_numpy(tensor):
            return tensor.detach().cpu().numpy() if tensor.requires_grad else tensor.cpu().numpy()

        tokenizer = transformers.RobertaTokenizer.from_pretrained(os.environ['UDF_TEST_DIR'] + "/exampleUDFs/roberta_tokenizer")
        input_ids = torch.tensor(tokenizer.encode(input, add_special_tokens=True)).unsqueeze(0)  # Batch size 1
        ort_inputs = {onnx_session.get_inputs()[0].name: to_numpy(input_ids)}
        return ort_inputs

    def tensor_to_output(tensor):
        import numpy as np
        pred = np.argmax(tensor)
        if (pred == 0):
            return("Negative")
        elif (pred == 1):
            return("Positive")

    # Workaround to properly start embedded python
    if not hasattr(sys, 'argv'):
        sys.argv  = ['']
    
    vectorsize = len(s)
    ret_arr = [None] * vectorsize
    for i, x in enumerate(s):
        if (x is None):
            ret_arr[i] = None
        else:
            ort_session = onnxruntime.InferenceSession(os.environ['UDF_TEST_DIR'] + "/exampleUDFs/roberta-sequence-classification.onnx")
            inputs = input_to_tensor(x, ort_session)
            ret = ort_session.run(None, inputs)
            ret_arr[i] = tensor_to_output(ret)

    return ret_arr

cdef public __udf_main(a1):
    return __udf_user(a1)
