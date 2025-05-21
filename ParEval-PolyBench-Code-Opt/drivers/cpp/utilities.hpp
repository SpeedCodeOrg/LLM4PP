#pragma once
#include <cassert>
#include <cmath>
#include <climits>
#include <cfloat>
#include <string>
#include <complex>
#include <queue>
#include <type_traits>
#include <set>
#include <utility>

#include <omp.h>
#include <iostream>

template <typename DType>
DType get_random_val_range(DType min, DType max, std::mt19937& engine) {
    DType val;
    if constexpr (std::is_floating_point_v<DType>) {
	std::uniform_real_distribution distrib(min, max);
	val = distrib(engine);
    } else if constexpr (std::is_integral_v<DType>) {
	std::uniform_int_distribution distrib(min, max - 1);
	val = distrib(engine);
    }
    return val;
}

// make sure some parallel model is defined
#if !defined(USE_SERIAL) && !defined(USE_OMP) && !defined(USE_MPI) && !defined(USE_MPI_OMP) && !defined(USE_KOKKOS) && !defined(USE_CUDA) && !defined(USE_HIP)
#error "No parallel model not defined"
#endif

// #define NO_OPTIMIZE __attribute__((optimize("O0")))
// #define NO_INLINE __attribute__((noinline)) __attribute__((optimize("O3")))
#define NO_OPTIMIZE
#define NO_INLINE

#if !defined(DRIVER_PROBLEM_SIZE)
#error "DRIVER_PROBLEM_SIZE not defined"
#endif

#if !defined(MAX_VALIDATION_ATTEMPTS)
#define MAX_VALIDATION_ATTEMPTS 1
#endif

#if !defined(SPARSE_LA_SPARSITY)
// sparsity to use for sparse linear algebra benchmarks
#define SPARSE_LA_SPARSITY 0.1
#endif

// include the necessary libraries for the parallel model
#if defined(USE_OMP) || defined(USE_MPI_OMP)
#include <omp.h>
#elif defined(USE_MPI) || defined(USE_MPI_OMP)
#include <mpi.h>
#elif defined(USE_KOKKOS)
#include <Kokkos_Core.hpp>
#elif defined(USE_CUDA)
#include <cuda_runtime.h>
#elif defined(USE_HIP)
#include <hip/hip_runtime.h>
#endif

// some helper macros to unify CUDA and HIP interfaces
#if defined(USE_CUDA)
#define GRID_STRIDE_LOOP(i, n) for (int i = blockIdx.x * blockDim.x + threadIdx.x; i < (n); i += blockDim.x * gridDim.x)
#define ALLOC(ptr, size) cudaMalloc(&(ptr), (size))
#define COPY_H2D(dst, src, size) cudaMemcpy((dst), (src), (size), cudaMemcpyHostToDevice)
#define COPY_D2H(dst, src, size) cudaMemcpy((dst), (src), (size), cudaMemcpyDeviceToHost)
#define FREE(ptr) cudaFree((ptr))
#define SYNC() cudaDeviceSynchronize()
#define DOUBLE_COMPLEX_T cuDoubleComplex
#define MAKE_DOUBLE_COMPLEX(r,i) make_cuDoubleComplex((r),(i))
#elif defined(USE_HIP)
#define GRID_STRIDE_LOOP(i, n) for (int i = hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x; i < (n); i += hipBlockDim_x * hipGridDim_x)
#define ALLOC(ptr, size) hipMalloc(&(ptr), (size))
#define COPY_H2D(dst, src, size) hipMemcpy((dst), (src), (size), hipMemcpyHostToDevice)
#define COPY_D2H(dst, src, size) hipMemcpy((dst), (src), (size), hipMemcpyDeviceToHost)
#define FREE(ptr) hipFree((ptr))
#define SYNC() hipDeviceSynchronize()
#define DOUBLE_COMPLEX_T hipDoubleComplex
#define MAKE_DOUBLE_COMPLEX(r,i) make_hipDoubleComplex((r),(i))
#endif

#if defined(USE_CUDA) || defined(USE_HIP)
__device__ double atomicMul(double* address, double val) { 
  unsigned long long int* address_as_ull = (unsigned long long int*)address; 
  unsigned long long int old = *address_as_ull, assumed; 
  do { 
    assumed = old; 
    old = atomicCAS(address_as_ull, assumed, __double_as_longlong(val * __longlong_as_double(assumed))); 
  } while (assumed != old); return __longlong_as_double(old);
} 
#endif

// Kokkos utilities
#if defined(USE_KOKKOS)
template <typename DType>
void copyVectorToView(std::vector<DType> const& vec, Kokkos::View<DType*> view) {
    assert(vec.size() == view.size());
    for (int i = 0; i < vec.size(); i += 1) {
        view(i) = vec[i];
    }
}

template <typename DType>
void copyViewToVector(Kokkos::View<DType*> view, std::vector<DType>& vec) {
    assert(vec.size() == view.size());
    for (int i = 0; i < vec.size(); i += 1) {
        vec[i] = view(i);
    }
}

template <typename DType>
void fillRandKokkos(Kokkos::View<DType*> &x, DType min, DType max) {
    for (int i = 0; i < x.size(); i += 1) {
        DType val;
        if constexpr (std::is_floating_point_v<DType>) {
            val = (rand() / (double) RAND_MAX) * (max - min) + min;
        } else if constexpr (std::is_integral_v<DType>) {
            val = rand() % (max - min) + min;
        }
        x(i) = val;
    }
}
#endif


// MPI utilities
#if defined(USE_MPI) || defined(USE_MPI_OMP)
#define IS_ROOT(rank) ((rank) == 0)
#define BCAST(vec,dtype) MPI_Bcast((vec).data(), (vec).size(), MPI_##dtype, 0, MPI_COMM_WORLD)
#define BCAST_PTR(ptr,size,dtype) MPI_Bcast(ptr, size, MPI_##dtype, 0, MPI_COMM_WORLD)
#define SYNC() MPI_Barrier(MPI_COMM_WORLD)
#define GET_RANK(rank) MPI_Comm_rank(MPI_COMM_WORLD, &(rank))
#else
#define IS_ROOT(rank) true
#define BCAST(vec,dtype)
#define BCAST_PTR(ptr,size,dtype)
#define GET_RANK(rank) rank = 0
#if !defined(SYNC)
#define SYNC()
#endif
#endif


/*
template <typename T>
void fillRandString(T &x, size_t minLen, size_t maxLen) {
    for (int i = 0; i < x.size(); i += 1) {
        size_t len = rand() % (maxLen - minLen) + minLen;
        std::string str(len, ' ');
        for (int j = 0; j < len; j += 1) {
            str[j] = 'a' + rand() % 26;
        }
        x[i] = str;
    }
}
*/

/*
template <typename T>
void fillRandString(T &x, size_t minLen, size_t maxLen, std::mt19937& engine) {
    for (int i = 0; i < x.size(); i += 1) {
        // size_t len = rand() % (maxLen - minLen) + minLen;
        size_t len = get_random_val_range(minLen, maxLen);
        std::string str(len, ' ');
        for (int j = 0; j < len; j += 1) {
            // str[j] = 'a' + rand() % 26;
            str[j] = 'a' + get_random_val_range(0, 26);
        }
        x[i] = str;
    }
}
*/

// utility functions
/*
template <typename T, typename DType>
void fillRand(T &x, DType min, DType max) {
    
    for (int i = 0; i < x.size(); i += 1) {
        DType val;
        if constexpr (std::is_floating_point_v<DType>) {
            val = (rand() / (double) RAND_MAX) * (max - min) + min;
        } else if constexpr (std::is_integral_v<DType>) {
            val = rand() % (max - min) + min;
        } else if constexpr (std::is_same_v<DType, std::complex<double>>) {
            const double real = (rand() / (double) RAND_MAX) * (max - min) + min;
            const double imag = (rand() / (double) RAND_MAX) * (max - min) + min;
            val = std::complex<double>(real, imag);
        }
        x[i] = val;
    }
}
*/

/*
template <typename T, typename DType>
void fillRand(T &x, DType min, DType max, std::mt19937& engine) {
    for (int i = 0; i < x.size(); i += 1) {
        DType val;
        if constexpr (std::is_floating_point_v<DType>) {
            // val = (rand() / (double) RAND_MAX) * (max - min) + min;
            val = get_random_val_range(min, max, engine);
        } else if constexpr (std::is_integral_v<DType>) {
            // val = rand() % (max - min) + min;
            val = get_random_val_range(min, max, engine);
        } else if constexpr (std::is_same_v<DType, std::complex<double>>) {
            const double real = get_random_val_range(min, max, engine);
            const double imag = get_random_val_range(min, max, engine);
            // const double real = (rand() / (double) RAND_MAX) * (max - min) + min;
            // const double imag = (rand() / (double) RAND_MAX) * (max - min) + min;
            val = std::complex<double>(real, imag);
        }
        x[i] = val;
    }
}
*/

// compare two vectors of floating point numbers
template <typename Vec, typename FType>
bool fequal(Vec const& a, Vec const& b, FType epsilon = 1e-6) {
    assert(a.size() == b.size());
    for (int i = 0; i < a.size(); i += 1) {
        if (std::abs(a[i] - b[i]) > epsilon || std::isnan(a[i]) || std::isnan(b[i])) {
            return false;
        }
    }
    return true;
}

void generateRandomGraph(std::vector<int>& A, int N, int num_edges, bool is_directed, std::mt19937& engine) {
    std::uniform_int_distribution<> vertex_dist(0, N - 1);

    for (int i = 0; i < num_edges; i++) {
	int u = vertex_dist(engine);
	int v = vertex_dist(engine);
	if (u != v) {
            A[u * N + v] = 1;
            if (!is_directed) {
                A[v * N + u] = 1;
            }
	}
    }
}

// Function to generate an R-MAT graph with any number of nodes
void generateRMATGraph(std::vector<int>& A, int N, int numEdges, double a, double b, double c, double d, bool is_directed, std::mt19937& engine) {
    std::fill(A.begin(), A.end(), 0);

    std::vector<std::pair<int, int>> edges;
    std::set<std::pair<int, int>> edgeSet;

    std::uniform_real_distribution<> dist(0, 1);
    std::uniform_int_distribution<> v_dist(0, N - 1);

    for (int i = 0; i < numEdges; ++i) {
        int u = v_dist(engine);
        int v = v_dist(engine);  // Start at a random node

        int step = N / 2;  // Start with the largest division step
        while (step > 0) {
            double r = dist(engine);

            if (r < a) {
                // Top-left quadrant
            } else if (r < a + b) {
                v += step;
            } else if (r < a + b + c) {
                u += step;
            } else {
                u += step;
                v += step;
            }

            step /= 2;
        }

        if (u >= N) u = v_dist(engine);  // Ensure u is within bounds
        if (v >= N) v = v_dist(engine);  // Ensure v is within bounds

        if (u != v && edgeSet.find({u, v}) == edgeSet.end()) {
            edges.emplace_back(u, v);
            edgeSet.insert({u, v});
        }
    }

    for (auto& [u, v] : edges) {
        A[u * N + v] = 1;
        if (!is_directed) {
            A[v * N + u] = 1;
        }
    }
}

int fillRandomUndirectedGraph_(std::vector<int> &A, size_t N, std::mt19937& engine) {
    std::fill(A.begin(), A.end(), 0);
    int max_edges = N * (N - 1) / 2;
    std::uniform_int_distribution<> e_dist(0, max_edges / 1000);
    int num_edges = e_dist(engine);

    double a = 0.45;
    double b = 0.15;
    double c = 0.15;
    double d = 0.25;
    
    // generateRMATGraph(A, N, num_edges, a, b, c, d, false, engine);
    generateRandomGraph(A, N, num_edges, false, engine);

    return num_edges;
}

int fillRandDirectedGraph_(std::vector<int> &A, size_t N, std::mt19937& engine) {
    std::fill(A.begin(), A.end(), 0);
    int max_edges = N * (N - 1);
    std::uniform_int_distribution<> e_dist(0, max_edges / 1000);
    int num_edges = e_dist(engine);

    double a = 0.45;
    double b = 0.15;
    double c = 0.15;
    double d = 0.25;
    
    // generateRMATGraph(A, N, num_edges, a, b, c, d, true, engine);
    generateRandomGraph(A, N, num_edges, true, engine);

    return num_edges;
}

