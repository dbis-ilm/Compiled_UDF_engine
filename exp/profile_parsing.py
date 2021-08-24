import subprocess
import re

def extract_times(p:subprocess.CompletedProcess):
    found = False
    for line in p.stdout.decode("utf-8").splitlines():
        if "UDF |" in line or "UDF(vec) |" in line:
            if found:
                print("Warning: found multiple UDFs in query profile")
                return None
            found = True
            build_time_str = re.search("BUILD: \d+\.\d+", line).group()
            build_time = float(re.search("\d+\.\d+", build_time_str).group())
            exec_time_str = re.search("EXECUTE: \d+\.\d+", line).group()
            exec_time = float(re.search("\d+\.\d+", exec_time_str).group())
    return [build_time, exec_time]