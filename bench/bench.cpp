#include "experiments.hpp"

#include "algorithms/brute_force.hpp"
#include "algorithms/dynamic_programming.hpp"
#include "algorithms/karmarkar_karp.hpp"
#include "algorithms/meet_in_the_middle.hpp"
#include "input.hpp"
#include "number.hpp"
#include "solve.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#ifndef BENCH_COMMIT
#define BENCH_COMMIT "unknown"
#endif

namespace {

constexpr double kMinimumTimedSeconds = 0.010;
constexpr double kOverLimitSeconds = 300.0;

struct Baseline {
    Verdict answer = Verdict::No;
    std::string d = "NA";
};

template <class V>
struct BenchResult {
    Verdict answer;
    std::string d;
    std::string decided_by;
};

std::string algorithm_name(Algorithm algorithm) {
    switch (algorithm) {
        case Algorithm::Solve: return "solve";
        case Algorithm::BruteForce: return "bf";
        case Algorithm::MeetInTheMiddle: return "mitm";
        case Algorithm::DynamicProgramming: return "dp";
        case Algorithm::KarmarkarKarp: return "kk";
    }
    return "unknown";
}

std::string arithmetic_name(Arithmetic arithmetic) {
    switch (arithmetic) {
        case Arithmetic::Native: return "native";
        case Arithmetic::Boost: return "boost";
        case Arithmetic::Own: return "own";
    }
    return "unknown";
}

std::string verdict_name(Verdict verdict) {
    switch (verdict) {
        case Verdict::Yes: return "YES";
        case Verdict::No: return "NO";
        case Verdict::Unknown: return "UNKNOWN";
    }
    return "UNKNOWN";
}

std::string method_name(Method method) {
    switch (method) {
        case Method::KarmarkarKarp: return "kk";
        case Method::OddSum: return "odd";
        case Method::DynamicProgramming: return "dp";
        case Method::MeetInTheMiddle: return "mitm";
        case Method::None: return "none";
    }
    return "NA";
}

std::string utc_date() {
    const auto now = std::chrono::system_clock::now();
    const std::time_t seconds = std::chrono::system_clock::to_time_t(now);
    std::tm utc {};
#ifdef _WIN32
    gmtime_s(&utc, &seconds);
#else
    gmtime_r(&seconds, &utc);
#endif
    std::ostringstream out;
    out << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ");
    return out.str();
}

template <class V>
V partition_sum(std::span<const V> values, const std::vector<std::size_t>& indices) {
    V sum = 0;
    for (std::size_t index : indices) sum = sum + values[index];
    return sum;
}

template <class V>
void verify_partition(std::span<const V> values, const std::vector<std::size_t>& a,
                      const std::vector<std::size_t>& b) {
    if (a.empty() || b.empty()) throw std::runtime_error("partition side is empty");
    std::vector<std::size_t> coverage = a;
    coverage.insert(coverage.end(), b.begin(), b.end());
    std::sort(coverage.begin(), coverage.end());
    if (coverage.size() != values.size()) throw std::runtime_error("partition does not cover the set");
    for (std::size_t i = 0; i < coverage.size(); ++i) {
        if (coverage[i] != i) throw std::runtime_error("partition indices are not disjoint");
    }
}

template <class V>
BenchResult<V> call_algorithm(Algorithm algorithm, std::span<const V> values) {
    if (algorithm == Algorithm::BruteForce) {
        const ExactAnswer<V> answer = brute_force<V>(values);
        if (!answer) return {Verdict::No, "NA", "NA"};
        verify_partition<V>(values, answer->A, answer->B);
        if (partition_sum<V>(values, answer->A) != partition_sum<V>(values, answer->B))
            throw std::runtime_error("brute-force partition is not balanced");
        return {Verdict::Yes, "NA", "NA"};
    }
    if (algorithm == Algorithm::MeetInTheMiddle) {
        const ExactAnswer<V> answer = meet_in_the_middle<V>(values);
        if (!answer) return {Verdict::No, "NA", "NA"};
        verify_partition<V>(values, answer->A, answer->B);
        if (partition_sum<V>(values, answer->A) != partition_sum<V>(values, answer->B))
            throw std::runtime_error("meet-in-the-middle partition is not balanced");
        return {Verdict::Yes, "NA", "NA"};
    }
    if (algorithm == Algorithm::DynamicProgramming) {
        const ExactAnswer<V> answer = dp_partition<V>(values);
        if (!answer) return {Verdict::No, "NA", "NA"};
        verify_partition<V>(values, answer->A, answer->B);
        if (partition_sum<V>(values, answer->A) != partition_sum<V>(values, answer->B))
            throw std::runtime_error("dynamic-programming partition is not balanced");
        return {Verdict::Yes, "NA", "NA"};
    }
    if (algorithm == Algorithm::KarmarkarKarp) {
        const Partition<V> answer = karmarkar_karp<V>(values);
        verify_partition<V>(values, answer.A, answer.B);
        const V sum_a = partition_sum<V>(values, answer.A);
        const V sum_b = partition_sum<V>(values, answer.B);
        if (sum_a == sum_b && answer.d != 0)
            throw std::runtime_error("Karmarkar-Karp discrepancy is inconsistent");
        if (sum_a != sum_b && answer.d == 0)
            throw std::runtime_error("Karmarkar-Karp discrepancy is zero incorrectly");
        const V difference = sum_a >= sum_b ? sum_a - sum_b : sum_b - sum_a;
        if (difference != answer.d)
            throw std::runtime_error("Karmarkar-Karp discrepancy is wrong");
        return {answer.d == 0 ? Verdict::Yes : Verdict::No, to_string(answer.d), "NA"};
    }

    const Solution<V> answer = solve<V>(values, Ranges{kDefaultNMitm, kDefaultTDp, kDefaultWDp});
    verify_partition<V>(values, answer.partition.A, answer.partition.B);
    if (answer.balanced) {
        const Partition<V>& balanced = *answer.balanced;
        verify_partition<V>(values, balanced.A, balanced.B);
        if (partition_sum<V>(values, balanced.A) != partition_sum<V>(values, balanced.B))
            throw std::runtime_error("SOLVE balanced partition is not balanced");
    }
    return {answer.answer, to_string(answer.partition.d), method_name(answer.decided_by)};
}

template <class V>
std::size_t largest_digits(std::span<const V> values) {
    std::size_t result = 0;
    for (const V& value : values)
        result = std::max(result, to_string(value).size());
    return result;
}

std::string row(const std::string& experiment, const std::string& algorithm,
                const std::string& arithmetic, const std::string& base,
                const std::string& file, const std::string& set,
                const std::string& n, const std::string& digits,
                const std::string& target, const std::string& rep,
                const std::string& calls, const std::string& time_s,
                const std::string& answer, const std::string& d,
                const std::string& decided_by, const std::string& status) {
    return experiment + "\t" + algorithm + "\t" + arithmetic + "\t" + base + "\t" +
           file + "\t" + set + "\t" + n + "\t" + digits + "\t" + target + "\t" +
           rep + "\t" + calls + "\t" + time_s + "\t" + answer + "\t" + d + "\t" +
           decided_by + "\t" + status;
}

template <class V>
void verify_known_answer(const ExperimentFile& file, Algorithm algorithm,
                         const BenchResult<V>& result) {
    // Karmarkar-Karp is the quality-measured approximation in E6: a positive
    // discrepancy on a constructed YES set is valid and must be recorded.
    if (algorithm == Algorithm::KarmarkarKarp) return;
    if (file.family == GeneratorFamily::Trap && result.answer != Verdict::No)
        throw std::runtime_error("trap set has an answer other than NO");
    if (file.family == GeneratorFamily::Yes && result.answer != Verdict::Yes)
        throw std::runtime_error("yes set has an answer other than YES");
}

template <class V>
bool baseline_matches(std::map<std::pair<std::string, std::size_t>, Baseline>& baselines,
                      const std::string& key, std::size_t set,
                      const BenchResult<V>& result) {
    const auto found = baselines.find({key, set});
    if (found == baselines.end()) {
        baselines[{key, set}] = {result.answer, result.d};
        return true;
    }
    return found->second.answer == result.answer &&
           (result.d == "NA" || found->second.d == "NA" ||
            found->second.d == result.d);
}

template <class V>
void run_typed(const Experiment& experiment, const ExperimentFile& file,
               Arithmetic arithmetic, std::uint64_t base,
               const std::vector<Algorithm>& algorithms,
               std::ofstream& out,
               std::map<std::pair<std::string, std::size_t>, Baseline>& baselines,
               bool& over_limit_seen,
               const std::chrono::steady_clock::time_point& file_start) {
    if (arithmetic == Arithmetic::Own) BigUint::set_base(base);

    const std::string path = "inputs/" + experiment.name + "/" + file.name;
    std::vector<Instance<V>> instances;
    try {
        instances = read_instances_from_file<V>(path);
    } catch (const InputError&) {
        out << row(experiment.name, algorithm_name(experiment.algorithm),
                   arithmetic_name(arithmetic), base == 0 ? "NA" : std::to_string(base),
                   file.name, "NA", "NA", "NA", "NA", "NA", "NA", "NA",
                   "NA", "NA", "NA", "unsupported") << '\n';
        return;
    }

    for (const Algorithm algorithm : algorithms) {
        if (experiment.name == "A4") {
            if (algorithm == Algorithm::KarmarkarKarp && file.n != 10000) continue;
            if (algorithm == Algorithm::MeetInTheMiddle && file.n != 30) continue;
        }

        if (over_limit_seen) break;

        for (std::size_t set = 0; set < instances.size(); ++set) {
            const std::span<const V> values(instances[set].values);
            const std::size_t n = values.size();
            const std::size_t digits = largest_digits<V>(values);
            const std::optional<std::uint64_t> target = to_u64(half(instances[set].sum));
            const std::string target_text = target ? std::to_string(*target) : "NA";

            BenchResult<V> warm_result;
            bool warm_valid = false;
            try {
                warm_result = call_algorithm<V>(algorithm, values);
                warm_valid = true;
                verify_known_answer<V>(file, algorithm, warm_result);
            } catch (const std::bad_alloc&) {
                for (int rep = 1; rep <= experiment.reps; ++rep) {
                    out << row(experiment.name, algorithm_name(algorithm),
                               arithmetic_name(arithmetic), base == 0 ? "NA" : std::to_string(base),
                               file.name, std::to_string(set + 1), std::to_string(n),
                               std::to_string(digits), target_text, std::to_string(rep),
                               "NA", "NA", "NA", "NA", "NA", "out_of_memory") << '\n';
                }
                continue;
            } catch (const std::runtime_error& error) {
                const std::string message = error.what();
                if (message.find("needs") != std::string::npos ||
                    message.find("memory") != std::string::npos ||
                    message.find("bad_alloc") != std::string::npos) {
                    for (int rep = 1; rep <= experiment.reps; ++rep) {
                        out << row(experiment.name, algorithm_name(algorithm),
                                   arithmetic_name(arithmetic), base == 0 ? "NA" : std::to_string(base),
                                   file.name, std::to_string(set + 1), std::to_string(n),
                                   std::to_string(digits), target_text, std::to_string(rep),
                                   "NA", "NA", "NA", "NA", "NA", "out_of_memory") << '\n';
                    }
                    continue;
                }
                throw std::runtime_error("verification failed in " + path +
                                         ", set " + std::to_string(set + 1) +
                                         ": " + message);
            } catch (const std::exception& error) {
                throw std::runtime_error("failure in " + path +
                                         ", set " + std::to_string(set + 1) +
                                         ": " + error.what());
            }

            std::uint64_t calls = 1;
            volatile std::uint64_t keepalive = 0;
            double per_call_seconds = 0.0;
            for (int rep = 1; rep <= experiment.reps; ++rep) {
                if (std::chrono::duration<double>(std::chrono::steady_clock::now() - file_start).count() >
                    kOverLimitSeconds) {
                    over_limit_seen = true;
                    out << row(experiment.name, algorithm_name(algorithm),
                               arithmetic_name(arithmetic), base == 0 ? "NA" : std::to_string(base),
                               file.name, std::to_string(set + 1), std::to_string(n),
                               std::to_string(digits), target_text, std::to_string(rep),
                               "0", "NA", "NA", "NA", "NA", "over_limit") << '\n';
                    continue;
                }

                if (rep == 1) {
                    const auto start = std::chrono::steady_clock::now();
                    calls = 1;
                    BenchResult<V> last;
                    for (;;) {
                        for (std::uint64_t i = 0; i < calls; ++i) {
                            last = call_algorithm<V>(algorithm, values);
                            keepalive += last.answer == Verdict::Yes ? 1 : 0;
                        }
                        const double elapsed =
                            std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
                        if (elapsed >= kMinimumTimedSeconds)
                            break;
                        calls *= 2;
                    }
                    verify_known_answer<V>(file, algorithm, last);
                    per_call_seconds =
                        std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() /
                        static_cast<double>(calls);
                } else {
                    const auto start = std::chrono::steady_clock::now();
                    BenchResult<V> last;
                    for (std::uint64_t i = 0; i < calls; ++i) {
                        last = call_algorithm<V>(algorithm, values);
                        keepalive += last.answer == Verdict::Yes ? 1 : 0;
                    }
                    verify_known_answer<V>(file, algorithm, last);
                    per_call_seconds =
                        std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count() /
                        static_cast<double>(calls);
                }

                BenchResult<V> checked =
                    warm_valid ? warm_result : call_algorithm<V>(algorithm, values);
                verify_known_answer<V>(file, algorithm, checked);
                if (!baseline_matches<V>(baselines,
                                         experiment.name + "/" + algorithm_name(algorithm) + "/" + file.name,
                                         set + 1, checked))
                    throw std::runtime_error("arithmetics disagree for " + path +
                                             ", set " + std::to_string(set + 1));

                std::ostringstream seconds;
                seconds << std::fixed << std::setprecision(9) << per_call_seconds;
                out << row(experiment.name, algorithm_name(algorithm),
                           arithmetic_name(arithmetic), base == 0 ? "NA" : std::to_string(base),
                           file.name, std::to_string(set + 1), std::to_string(n),
                           std::to_string(digits), target_text, std::to_string(rep),
                           std::to_string(calls), seconds.str(),
                           verdict_name(checked.answer), checked.d,
                           checked.decided_by, "ok") << '\n';
            }
            (void)keepalive;

            if (std::chrono::duration<double>(std::chrono::steady_clock::now() - file_start).count() >
                kOverLimitSeconds) {
                over_limit_seen = true;
            }
        }
    }
}

std::string result_header() {
    return "experiment\talgorithm\tarithmetic\tbase\tfile\tset\tn\tdigits\tT\trep\tcalls\t"
           "time_s\tanswer\td\tdecided_by\tstatus";
}

void write_machine_file() {
    std::filesystem::create_directories("results");
    std::ofstream out("results/machine.txt", std::ios::binary);
    out << "system: ";
#ifdef _WIN32
    out << "Windows";
#else
    out << "Linux";
#endif
    out << '\n';
    out << "threads: " << std::thread::hardware_concurrency() << '\n';
    out << "compiler: " << __VERSION__ << '\n';
    out << "date: " << utc_date() << '\n';
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 2) {
        std::cerr << "usage: bench [E1|E2|E3a|E3b|E4|E5|E6|E7|A1|A2|A3|A4]\n";
        return 2;
    }
    std::string selected;
    if (argc == 2) selected = argv[1];

    try {
        std::filesystem::create_directories("results");
        write_machine_file();
        const auto run_start = std::chrono::steady_clock::now();

        bool found = selected.empty();
        for (const Experiment& experiment : experiments::kExperiments) {
            if (!selected.empty() && experiment.name == selected) found = true;
            if (!selected.empty() && experiment.name != selected) continue;
        const std::string result_path = "results/" + experiment.name + ".tsv";
        std::ofstream out(result_path, std::ios::binary | std::ios::trunc);
        if (!out) {
            std::cerr << "cannot write " << result_path << '\n';
            return 4;
        }
        out << "# experiment: " << experiment.name << '\n';
        out << "# date: " << utc_date() << '\n';
        out << "# commit: " << BENCH_COMMIT << '\n';
        out << "# compiler: " << __VERSION__ << " (flags: -std=c++20 -O2)\n";
        out << "# generator seed: 20261204\n";
        out << result_header() << '\n';

        std::map<std::pair<std::string, std::size_t>, Baseline> baselines;
        for (const ArithmeticRun& run : experiment.arithmetics) {
            bool over_limit_seen = false;
            for (const ExperimentFile& file : experiment.files) {
                if (over_limit_seen) break;
                const auto file_start = std::chrono::steady_clock::now();
                std::cout << experiment.name << " " << arithmetic_name(run.arithmetic)
                          << " " << file.name << " 0.000 s\n";
                std::vector<Algorithm> algorithms =
                    experiment.algorithms.empty()
                        ? std::vector<Algorithm>{experiment.algorithm}
                        : experiment.algorithms;

                if (run.arithmetic == Arithmetic::Native) {
                    run_typed<Native>(experiment, file, run.arithmetic, run.base, algorithms,
                                      out, baselines, over_limit_seen, file_start);
                } else if (run.arithmetic == Arithmetic::Boost) {
                    run_typed<BoostInt>(experiment, file, run.arithmetic, run.base, algorithms,
                                        out, baselines, over_limit_seen, file_start);
                } else {
                    run_typed<BigUint>(experiment, file, run.arithmetic, run.base, algorithms,
                                       out, baselines, over_limit_seen, file_start);
                }

                const double seconds =
                    std::chrono::duration<double>(std::chrono::steady_clock::now() - file_start).count();
                std::cout << experiment.name << " " << arithmetic_name(run.arithmetic)
                          << " " << file.name << " " << std::fixed << std::setprecision(3)
                          << seconds << " s\n";
                out.flush();
            }
        }
        }

        const double seconds =
            std::chrono::duration<double>(std::chrono::steady_clock::now() - run_start).count();
        std::cout << "total: " << std::fixed << std::setprecision(3) << seconds << " s\n";
        if (!found) {
            std::cerr << "unknown experiment: " << selected << '\n';
            return 2;
        }
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "bench: fatal: " << error.what() << '\n';
        std::cerr << "See the current TSV file for completed rows.\n";
        return 1;
    } catch (...) {
        std::cerr << "bench: fatal: unknown exception\n";
        return 1;
    }
}
