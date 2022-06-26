![build](https://github.com/wsandst/rdu/actions/workflows/ci.yml/badge.svg?branch=main)
# Rapid Disk Usage
`rdu` is a command line utility for determining disk usage of folders and files recursively, designed for maximal speed. 
The tool is very similar to the built-in linux utility `du`. By utilizing threads and better system calls, the performance has been increased significantly.

# Command-line arguments
`rdu` supports most of the commands from `du`. Some additional commands have been added, for example specifying thread count, cache usage or percentage display.

Usage: `rdu [OPTION] [FILE1] [...]`  
Usage example: `rdu /etc/ -hs`

### Supported plain `du` flags
    -h, --human-readable
    -s, --summarize: Only display the size of the passed folders
    -a, --all: Show every file and folder
    -t, --time: Include the last modification time
    -c, --total: Show total at the end under 'total', same as summarize
    -d, --max-depth=N: Max depth to print size of, max-depth=0 is same as summarize
    -L, --dereference: dereference symbolic links
    -D, --dereference-args, also -H: deference only symbolic links sent directly in command-line as folders to check
    -B, --block-size=SIZE: scale sizes by SIZE before printing them
    
### Additional added flags
    -j, --threads: max amount of threads to use. Default is to use logical core count
    -C, --create-cache: Create a new cache file. This will be stored in /tmp/ by default, or in a user specified location
    -u, --use-cache: Use a created file cache. This will greatly speed up retrieving of disk usage, but this assumes that none of the files have changed since the cache creation, otherwise the results will be wrong!
    -m, --min-size: The minimum size of a folder to display. This can be in plain bytes, human readable or percentage.

# Benchmarks
The benchmarks have been performed with [Hyperfine](https://github.com/sharkdp/hyperfine).

## Comparison with other 'du' alternatives
| Command | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| `du` | 1.545 ± 0.010 | 1.529 | 1.564 | 9.51 ± 0.20 |
| `mdu st (Harald)` | 1.432 ± 0.012 | 1.418 | 1.456 | 8.81 ± 0.19 |
| `mdu st (William)` | 1.261 ± 0.009 | 1.247 | 1.282 | 7.76 ± 0.16 |
| `tin-summer st` | 1.484 ± 0.011 | 1.467 | 1.496 | 9.13 ± 0.19 |
| `tin-summer mt` | 0.529 ± 0.026 | 0.472 | 0.562 | 3.26 ± 0.17 |
| `diskus mt` | 0.471 ± 0.022 | 0.437 | 0.496 | 2.90 ± 0.15 |
| `mdu mt (Harald)` | 0.170 ± 0.003 | 0.166 | 0.175 | 1.05 ± 0.03 |
| `mdu mt (William)` | 0.163 ± 0.004 | 0.158 | 0.172 | 1.00 ± 0.03 |
| `rdu mt (William)` | 0.163 ± 0.003 | 0.158 | 0.169 | 1.00 |


## Performance with varying thread counts
| Threads | Mean [s] | Min [s] | Max [s] | Relative |
|:---|---:|---:|---:|---:|
| 1 Threads | 1.164 ± 0.019 | 1.140 | 1.198 | 7.73 ± 0.14 |
| 2 Threads | 0.655 ± 0.010 | 0.644 | 0.670 | 4.35 ± 0.07 |
| 3 Threads | 0.473 ± 0.006 | 0.462 | 0.482 | 3.14 ± 0.05 |
| 4 Threads | 0.370 ± 0.005 | 0.363 | 0.377 | 2.46 ± 0.04 |
| 5 Threads | 0.320 ± 0.014 | 0.303 | 0.349 | 2.12 ± 0.09 |
| 6 Threads | 0.277 ± 0.008 | 0.269 | 0.292 | 1.84 ± 0.05 |
| 7 Threads | 0.251 ± 0.008 | 0.241 | 0.268 | 1.67 ± 0.06 |
| 8 Threads | 0.221 ± 0.002 | 0.219 | 0.225 | 1.47 ± 0.02 |
| 9 Threads | 0.207 ± 0.002 | 0.205 | 0.212 | 1.38 ± 0.02 |
| 10 Threads | 0.199 ± 0.004 | 0.194 | 0.207 | 1.32 ± 0.03 |
| 11 Threads | 0.186 ± 0.002 | 0.184 | 0.191 | 1.24 ± 0.02 |
| 12 Threads | 0.176 ± 0.001 | 0.175 | 0.177 | 1.17 ± 0.01 |
| 13 Threads | 0.168 ± 0.001 | 0.167 | 0.173 | 1.12 ± 0.01 |
| 14 Threads | 0.162 ± 0.001 | 0.161 | 0.164 | 1.07 ± 0.01 |
| 15 Threads | 0.156 ± 0.002 | 0.154 | 0.161 | 1.04 ± 0.01 |
| 16 Threads | 0.151 ± 0.001 | 0.150 | 0.155 | 1.00 ± 0.01 |
| 17 Threads | 0.151 ± 0.001 | 0.149 | 0.153 | 1.00 |
| 18 Threads | 0.151 ± 0.003 | 0.149 | 0.162 | 1.00 ± 0.02 |
| 19 Threads | 0.152 ± 0.002 | 0.150 | 0.157 | 1.01 ± 0.02 |
| 20 Threads | 0.154 ± 0.004 | 0.149 | 0.162 | 1.02 ± 0.03 |
| 21 Threads | 0.156 ± 0.005 | 0.151 | 0.168 | 1.03 ± 0.04 |
| 22 Threads | 0.153 ± 0.002 | 0.150 | 0.157 | 1.02 ± 0.01 |
| 23 Threads | 0.153 ± 0.002 | 0.151 | 0.157 | 1.02 ± 0.02 |
| 24 Threads | 0.160 ± 0.007 | 0.152 | 0.170 | 1.06 ± 0.05 |
| 25 Threads | 0.159 ± 0.003 | 0.155 | 0.164 | 1.06 ± 0.02 |
| 26 Threads | 0.161 ± 0.002 | 0.157 | 0.167 | 1.07 ± 0.02 |
| 27 Threads | 0.157 ± 0.009 | 0.150 | 0.182 | 1.04 ± 0.06 |
| 28 Threads | 0.152 ± 0.002 | 0.149 | 0.159 | 1.01 ± 0.02 |
| 29 Threads | 0.155 ± 0.006 | 0.150 | 0.176 | 1.03 ± 0.04 |
| 30 Threads | 0.155 ± 0.005 | 0.150 | 0.168 | 1.03 ± 0.03 |
| 31 Threads | 0.158 ± 0.006 | 0.151 | 0.170 | 1.05 ± 0.04 |
| 32 Threads | 0.156 ± 0.007 | 0.149 | 0.173 | 1.04 ± 0.05 |
| 33 Threads | 0.153 ± 0.002 | 0.150 | 0.158 | 1.01 ± 0.02 |
| 34 Threads | 0.153 ± 0.002 | 0.149 | 0.157 | 1.01 ± 0.02 |
| 35 Threads | 0.153 ± 0.002 | 0.150 | 0.157 | 1.02 ± 0.02 |
| 36 Threads | 0.155 ± 0.005 | 0.151 | 0.172 | 1.03 ± 0.03 |
| 37 Threads | 0.154 ± 0.004 | 0.150 | 0.166 | 1.02 ± 0.03 |
| 38 Threads | 0.154 ± 0.005 | 0.149 | 0.175 | 1.02 ± 0.04 |
| 39 Threads | 0.153 ± 0.003 | 0.150 | 0.161 | 1.02 ± 0.02 |
| 40 Threads | 0.155 ± 0.004 | 0.150 | 0.168 | 1.03 ± 0.03 |
| 41 Threads | 0.156 ± 0.004 | 0.150 | 0.163 | 1.03 ± 0.03 |
| 42 Threads | 0.155 ± 0.004 | 0.149 | 0.163 | 1.03 ± 0.03 |
| 43 Threads | 0.154 ± 0.002 | 0.151 | 0.158 | 1.02 ± 0.02 |
| 44 Threads | 0.156 ± 0.002 | 0.151 | 0.160 | 1.03 ± 0.02 |
| 45 Threads | 0.155 ± 0.005 | 0.150 | 0.168 | 1.03 ± 0.03 |
| 46 Threads | 0.155 ± 0.004 | 0.151 | 0.164 | 1.03 ± 0.03 |
| 47 Threads | 0.160 ± 0.009 | 0.151 | 0.188 | 1.06 ± 0.06 |
| 48 Threads | 0.164 ± 0.004 | 0.159 | 0.174 | 1.09 ± 0.03 |
| 49 Threads | 0.157 ± 0.004 | 0.152 | 0.164 | 1.04 ± 0.03 |
| 50 Threads | 0.156 ± 0.005 | 0.152 | 0.171 | 1.04 ± 0.03 |
| 51 Threads | 0.158 ± 0.006 | 0.150 | 0.168 | 1.05 ± 0.04 |
| 52 Threads | 0.164 ± 0.007 | 0.153 | 0.181 | 1.09 ± 0.05 |
| 53 Threads | 0.157 ± 0.004 | 0.153 | 0.167 | 1.04 ± 0.03 |
| 54 Threads | 0.157 ± 0.003 | 0.151 | 0.163 | 1.04 ± 0.02 |
| 55 Threads | 0.157 ± 0.003 | 0.151 | 0.165 | 1.04 ± 0.02 |
| 56 Threads | 0.158 ± 0.005 | 0.152 | 0.170 | 1.05 ± 0.03 |
| 57 Threads | 0.160 ± 0.006 | 0.151 | 0.174 | 1.06 ± 0.04 |
| 58 Threads | 0.157 ± 0.004 | 0.152 | 0.164 | 1.04 ± 0.02 |
| 59 Threads | 0.160 ± 0.005 | 0.152 | 0.170 | 1.06 ± 0.03 |
| 60 Threads | 0.161 ± 0.007 | 0.151 | 0.177 | 1.07 ± 0.05 |
| 61 Threads | 0.166 ± 0.010 | 0.153 | 0.190 | 1.10 ± 0.07 |
| 62 Threads | 0.164 ± 0.008 | 0.156 | 0.186 | 1.09 ± 0.05 |
| 63 Threads | 0.169 ± 0.009 | 0.156 | 0.187 | 1.12 ± 0.06 |
| 64 Threads | 0.160 ± 0.005 | 0.153 | 0.169 | 1.06 ± 0.03 |
