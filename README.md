# Compiled_UDF_engine
Code for the engine implementation and reproducibility of the evaluation in paper (LINK). Allows the dynamic loading of just-in-time compiled Python UDF code into a running vectorized query engine. 

# Building and running experiments
Just use the provided Dockerfile to generate the environment for the experiments.
```
docker build -t compiledUDFs .
docker run -it compiledUDFs
```
In the root folder of the running docker container, run 
```
BUILDMODE=RELEASE . build.sh
```
to build the engine in release mode.

Run tests using 
```
cd /release 
make test
```
or run the experiments using
```
cd /exp
python3.6 run_experiments <number_of_runs>
```
which prints the dataframe normalized against python execution time, writes all measurements to `/tmp/results.csv` and saves the generated plot from the paper in `/tmp/results.png`.
