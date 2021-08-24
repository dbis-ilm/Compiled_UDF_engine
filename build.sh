#!/bin/bash

dir=$(pwd)

if ! [[ "$BUILDMODE" != "RELEASE" || "$BUILDMODE" != "DEBUG" ]]; then
    echo "BUILDMODE not set. Please set to RELEASE or DEBUG."
    return 0
fi

if [ "$BUILDMODE" = "RELEASE" ]; then
    export BUILD_DIR="$dir/release"
    type="Release"
fi

if [ "$BUILDMODE" = "DEBUG" ]; then
    export BUILD_DIR="$dir/debug"
    type="Debug"
fi

rm -rf $BUILD_DIR
mkdir $BUILD_DIR
cd $BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=$type -B$BUILD_DIR $dir
make

export UDF_TEST_DIR="$dir/test"
export UDFUTILS="$dir/src/utils"

if [ ! -f "$UDF_TEST_DIR/exampleUDFs/roberta-sequence-classification.onnx" ]; then
    wget -O $UDF_TEST_DIR/exampleUDFs/roberta-sequence-classification.onnx https://github.com/onnx/models/blob/master/text/machine_comprehension/roberta/model/roberta-sequence-classification-9.onnx?raw=true
fi

export UDF_EXP_DIR="$dir/exp"

if [ ! -d "$UDF_EXP_DIR/tpch-dbgen/" ]; then
    echo -e "\n\nCheckout TPC-H sources\n\n"
    cd $UDF_EXP_DIR
    git clone https://github.com/electrum/tpch-dbgen.git
    cd tpch-dbgen
    make 2>/dev/null
    if [ ! -f "$UDF_EXP_DIR/tpch-dbgen/partsupp.tbl" ]; then
        ./dbgen -s 10 -T S
    fi
fi

cd $dir