import subprocess
from concurrent.futures import ThreadPoolExecutor
from subprocess import CalledProcessError
import os


def build_sim(config_path, cwd):
    subprocess.run(f"./config.sh {config_path}", cwd=cwd, shell=True, check=True)
    subprocess.run("make -j8", cwd=cwd, shell=True, check=True)


def run_trace(trace, output, cwd, warmup, instrs_count):
    print(f"Running {trace}...........................")
    try:
        subprocess.run(
            f"./bin/champsim --warmup_instructions {warmup} --simulation_instructions {instrs_count} {trace} --json {output}",
            cwd=cwd,
            capture_output=True,
            shell=True,
            check=True,
        )
        print(f"The results are recorded in {output}.")
    except CalledProcessError as exc:
        print(exc.stderr)
        raise


def run_traces(dir, output_dir, cwd, warmup, instrs_count):
    current_cwd = os.getcwd()
    os.chdir(cwd)
    os_dir = os.fsencode(dir)
    with ThreadPoolExecutor(max_workers=16) as executor:
        for file in os.listdir(os_dir):
            file = file.decode("ascii")
            if file.endswith(".xz"):
                res_file = f"{output_dir}/{file}.json"
                executor.submit(
                    run_trace, f"{dir}/{file}", res_file, cwd, warmup, instrs_count
                )
    os.chdir(current_cwd)
