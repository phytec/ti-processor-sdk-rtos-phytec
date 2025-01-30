/******************************************************************************
*                                                                             *
* module name       :FFTLIB                                                   *
*                                                                             *
* module descripton :Matrix Multiply Accelerator Library module for C7x+MMA   *
*                                                                             *
* Copyright (C) 2017-2018 Texas Instruments Incorporated - http://www.ti.com/ *
* ALL RIGHTS RESERVED                                                         *
*                                                                             *
******************************************************************************/

/********************************************************************************
  *  @file     FFTLIB_FFTLIB__highRadixDecompositions_ixX_cxX_oxX_priv.h
  *
  *  @brief    File to hold private parameter of function FFTLIB_FFT_highRadixDecompositions
  *
  *  @version  0.1 - Jan 2018 : Initial Version
  *            
 ********************************************************************************/

#ifndef FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_PRIV_H_
#define FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_PRIV_H_

#include "FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX.h"
#include "../../common/FFTLIB_utility.h"

/*!
 * \file
 * \brief Header file for kernel's internal use. For the kernel's interface, please 
 *        see @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX
 */

/*! 
 * @brief Macro to define the size of bufPblock array of 
 *        @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs structure. 
 *
 */
#define FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_PBLOCK_SIZE (24*SE_PARAM_SIZE + 5*FFTLIB_PARAM_SIZE + 3*sizeof(uchar64) + 1*sizeof(uint32_t))

/*!
 *  @brief This is a function pointer type that conforms to the 
 *         declaration of 
 *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci
 *         and @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn.
 */
typedef FFTLIB_STATUS  (*pFxnFFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec)(
   FFTLIB_kernelHandle handle, 
   void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/*!
 *  @brief This function is the initialization function for the C7x 
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init.
 *
 * @details This function determines the configuration for the streaming engine
 *          and MMA hardware resources based on the function call parameters, 
 *          and the configuration is saved in bufPBlock array. In the kernel
 *          call sequence, 
 *          @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci would be
 *          called later independently by the application. When 
 *          @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci runs, it 
 *          merely retrieves the configuration from the bufPBlock and uses it to 
 *          set up the hardware resources. This arrangement is so that 
 *          @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci does not 
 *          lose cycles to determine the hardware configuration.
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer 
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 */
template <uint32_t dataType>
extern FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci(
   FFTLIB_kernelHandle handle,
   const FFTLIB_bufParams1D_t *bufParamsX,
   const FFTLIB_bufParams1D_t *bufParamsW,
   const FFTLIB_bufParams1D_t *bufParamsY,
   const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs);

/*!
 *  @brief This function is the main execution function for the C7x 
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec.
 *
 * @details The kernel is the optimized implementation for the Discrete Fourier
 *          Transform (DFT) computation using the C7x and MMA resources. The 
 *          FFT size (number of FFT coefficients) is assumed to be greater than
 *          or equal to (MMA vector width)*2, and is assumed to be a power of 2.
 *          
 *          DFT computation involves multiplication of the data vector by the
 *          DFT matrix. Our algorithm for DFT computation uses a factorization 
 *          of the DFT matrix into sparse matrices to carry out the computation
 *          efficiently.
 *
 *          The following diagram illustrates the factorization of an example 
 *          DFT matrix. A \f$ 16 \f$-point DFT matrix is taken as an example,
 *          and its factorization using a decomposition factor of \f$ 4 \f$
 *          into a product of three sparse matrices is shown. In the following
 *          diagram:
 *          - each matrix is bordered by edge rows/columns showing the 
 *          column/row indices.
 *          - the values in each matrix cell indicate exponent of 
 *          \f$ \omega \f$. Thus, \f$ 0 \f$ actually indicates 
 *          \f$ \omega^0 = 1 \f$.
 *          - An empty matrix cell indicates a value of \f$ 0 \f$ in the cell.
 *
 *          \image html highRadixDecompositions_dft_matrix_decomposition.svg
 *          \image latex highRadixDecompositions_dft_matrix_decomposition.eps
 *
 *          The last row shows the result of factorization. Here, the 
 *          right-most matrix is the sparse decomposition matrix which is a 
 *          square block matrix and each block is a diagonal matrix, the matrix
 *          is the middle is a block diagonal matrix where each block is the 
 *          \f$ 4 \f$-point DFT matrix, and the left-most matrix is the 
 *          permutation matrix. Now, instead of multiplying the data vector by 
 *          the original \f$ 16 \f$-point DFT matrix, we can multiply it in turn 
 *          by the sparse decomposition matrix, the block diagonal DFT matrix
 *          and the sparse permutation matrix. Multiplication by the sequence
 *          of sparse matrices is often computationally more efficient than 
 *          multiplying by the original dense matrix. After decomposition, if 
 *          the DFT size in each block of the block diagonal DFT matrix is 
 *          still too large to compute efficiently by direct matrix 
 *          multiplication, the block diagonal DFT matrix is itself further 
 *          decomposed (factorized) into a product of sparse matrices. The 
 *          decomposition continues through a sequence of stages until the DFT 
 *          size is small enough.
 *
 *          In our implementation, if the size of the input data vector is a 
 *          power of MMA vector width/2, a decomposition factor of 
 *          MMA vector width/2 is used in each decomposition stage. If the size
 *          of the input data vector is not a power of MMA vector width/2, a 
 *          decomposition factor less than MMA vector width/2 is used for the
 *          very first decomposition stage, while the subsequent decomposition
 *          stages just use a decomposition factor of MMA vector width/2. The 
 *          decomposition continues through stages until the DFT size is equal
 *          to MMA vector width/2. A decomposition factor of MMA vector width/2
 *          and the final DFT size of MMA vector width/2 are chosen because the
 *          data used is complex (with real and imaginary parts), and 
 *          MMA vector width/2 number of data points occupy a full MMA vector 
 *          width. 
 *
 *          We use the following notations to capture example factorizations in 
 *          mathematical expressions: 
 *          - \f$ \mathbf{F}_{N} \f$ indicates an \f$ N \f$-point DFT matrix of
 *          size \f$ N\times N \f$. In the above diagram, the \f$ 16 \f$-point
 *          DFT matrix shown is \f$ \mathbf{F}_{16} \f$.
 *          - \f$ \mathbf{P}_{N} \f$ indicates the permutation matrix of size 
 *          \f$ N\times N \f$. In the above diagram, the sparse permutation 
 *          matrix shown is \f$ \mathbf{P}_{16} \f$.
 *          - \f$ \mathbf{D}_{M,N} \f$ indicates the sparse decomposition 
 *          matrix with \f$ M\times M \f$ blocks, and each block is an 
 *          \f$ N\times N \f$ diagonal matrix. In the above diagram, the 
 *          sparse decomposition matrix shown is \f$ \mathbf{D}_{4,4} \f$.
 *          - \f$ \mathbf{A}^{k} \f$ indicates a square matrix 
 *          \f$ \mathbf{A} \f$ replicated block diagonally \f$ k \f$ times to 
 *          form a block diagonal matrix. In the above diagram, the block 
 *          diagonal DFT matrix shown is \f$ (\mathbf{F}_4)^4 \f$.
 *          - \f$ \mathbf{R}_N \f$ is used to denote the product of all 
 *          permutation matrices in the final decomposition of \f$ \mathbf{F}_N \f$.
 *
 *          In the following examples, a MMA vector width of \f$ 32 \f$ is 
 *          assumed so that the decomposition stages continue until the DFT
 *          size is equal to \f$ 16 \f$ - that is, until the DFT matrix 
 *          block in the block diagonal DFT matrix is of size \f$ 16\times 16 \f$.
 *
 *          Factorization of \f$ 256 \f$-point DFT matrix:
 *
 *          \f$ \begin{aligned} \mathbf{F}_{256} & = \mathbf{P}_{256}\left(\mathbf{F}_{16}\right)^{16}\mathbf{D}_{16,16} & \mbox{(decomposition stage 1)} \\ & = \mathbf{R}_{256}\left(\mathbf{F}_{16}\right)^{16}\mathbf{D}_{16,16} & \end{aligned}\f$
 *          <br><br>
 *          Factorization of \f$ 512 \f$-point DFT matrix:
 *
 *          \f$ \begin{aligned}\mathbf{F}_{512} & = \mathbf{P}_{512}\left(\mathbf{F}_{256}\right)^{2}\mathbf{D}_{2,256} & \mbox{(decomposition stage 1)}\\ & = \mathbf{P}_{512}\left(\mathbf{P}_{256}\left(\mathbf{F}_{16}\right)^{16}\mathbf{D}_{16,16}\right)^{2}\mathbf{D}_{2,256} & \mbox{(decomposition stage 2)}\\ & = \mathbf{P}_{512}\left(\mathbf{P}_{256}\right)^{2}\left(\mathbf{F}_{16}\right)^{32}\left(\mathbf{D}_{16,16}\right)^{2}\mathbf{D}_{2,256} & \\  & = \mathbf{R}_{512}\left(\mathbf{F}_{16}\right)^{32}\left(\mathbf{D}_{16,16}\right)^{2}\mathbf{D}_{2,256} & \end{aligned} \f$
 *          <br><br>
 *          Factorization of \f$ 1024 \f$-point DFT matrix:
 *
 *          \f$ \begin{aligned}\mathbf{F}_{1024} & = \mathbf{P}_{1024}\left(\mathbf{F}_{256}\right)^{4}\mathbf{D}_{4,256} & \mbox{(decomposition stage 1)}\\ & = \mathbf{P}_{1024}\left(\mathbf{P}_{256}\left(\mathbf{F}_{16}\right)^{16}\mathbf{D}_{16,16}\right)^{4}\mathbf{D}_{4,256} & \mbox{(decomposition stage 2)}\\ & = \mathbf{P}_{1024}\left(\mathbf{P}_{256}\right)^{4}\left(\mathbf{F}_{16}\right)^{64}\left(\mathbf{D}_{16,16}\right)^{4}\mathbf{D}_{4,256} & \\  & = \mathbf{R}_{1024}\left(\mathbf{F}_{16}\right)^{64}\left(\mathbf{D}_{16,16}\right)^{4}\mathbf{D}_{4,256} & \end{aligned} \f$
 *          <br><br>
 *          Factorization of \f$ 4096 \f$-point DFT matrix:
 *
 *          \f$ \begin{aligned}\mathbf{F}_{4096} & = \mathbf{P}_{4096}\left(\mathbf{F}_{256}\right)^{16}\mathbf{D}_{16,256} & \mbox{(decomposition stage 1)}\\ & = \mathbf{P}_{4096}\left(\mathbf{P}_{256}\left(\mathbf{F}_{16}\right)^{16}\mathbf{D}_{16,16}\right)^{16}\mathbf{D}_{16,256} & \mbox{(decomposition stage 2)}\\ & = \mathbf{P}_{4096}\left(\mathbf{P}_{256}\right)^{16}\left(\mathbf{F}_{16}\right)^{256}\left(\mathbf{D}_{16,16}\right)^{16}\mathbf{D}_{16,256} & \\ & = \mathbf{R}_{4096}\left(\mathbf{F}_{16}\right)^{256}\left(\mathbf{D}_{16,16}\right)^{16}\mathbf{D}_{16,256} & \end{aligned} \f$
 *          <br><br>
 *          Factorization of \f$ 8192 \f$-point DFT matrix:
 *
 *          \f$ \begin{aligned}\mathbf{F}_{8192} & = \mathbf{P}_{8192}\left(\mathbf{F}_{4096}\right)^2 \mathbf{D}_{2, 4096} & \mbox{(decomposition stage 1)} \\ & = \mathbf{P}_{8192}\left(\mathbf{P}_{4096}\left(\mathbf{F}_{256}\right)^{16}\mathbf{D}_{16,256}\right)^2 \mathbf{D}_{2,4096} & \mbox{(decomposition stage 2)}\\ & = \mathbf{P}_{8192}\left(\mathbf{P}_{4096}\right)^2\left(\mathbf{F}_{256}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2 \mathbf{D}_{2,4096} & \\ & = \mathbf{P}_{8192}\left(\mathbf{P}_{4096}\right)^2\left(\mathbf{P}_{256} \left(\mathbf{F}_{16}\right)^{16}\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2 \mathbf{D}_{2,4096} & \mbox{(decomposition stage 3)}\\ & = \mathbf{P}_{8192}\left(\mathbf{P}_{4096}\right)^2\left(\mathbf{P}_{256}\right)^{32}\left(\mathbf{F}_{16}\right)^{512}\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2 \mathbf{D}_{2,4096} & \\  & = \mathbf{R}_{8192}\left(\mathbf{F}_{16}\right)^{512}\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2 \mathbf{D}_{2,4096} & \end{aligned} \f$
 *          <br><br>
 *          The DFT computation is carried out by first multiplying the data 
 *          vector by the sparse decomposition \f$ \mathbf{D} \f$ matrices (in 
 *          as many stages as there are \f$ \mathbf{D} \f$ matrices in the final
 *          factorization), then multiplying the resultant data by the block 
 *          diagonal DFT matrix \f$ \left(\mathbf{F}_{M,N}\right)^k \f$, and 
 *          finally permuting the resultant data according to into the 
 *          \f$ \mathbf{R} \f$ matrix.
 *          <br><br>
 *          For example, to compute DFT for a \f$ 8192 \f$-point data vector 
 *          \f$ \mathbf{f} \f$ - that is, to compute
 *          \f$ \mathbf{F}_{8192}\times \mathbf{f} \f$, we use the sparse
 *          factorization
 *          \f$ \mathbf{F}_{8192} = \mathbf{R}_{8192}\left(\mathbf{F}_{16}\right)^{512}\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2 \mathbf{D}_{2,4096} \f$
 *          given above. The computation goes through different stages as
 *          follows:
 *
 *          1.Decomposition stage 1: \f$ \mathbf{D}_{2,4096}\times \mathbf{f} \f$
 *          <br>
 *          2.Decomposition stage 2: \f$ \left(\mathbf{D}_{16,256}\right)^2\times\left(\mathbf{D}_{2,4096}\mathbf{f}\right) \f$
 *          <br>
 *          3.Decomposition stage 3: \f$ \left(\mathbf{D}_{16,16}\right)^{32}\times\left(\left(\mathbf{D}_{16,256}\right)^2\mathbf{D}_{2,4096}\mathbf{f}\right) \f$
 *          <br>
 *          4.DFTs' stage (\f$ 16 \f$-point DFTs): \f$ \left(\mathbf{F}_{16}\right)^{512}\times\left(\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2\mathbf{D}_{2,4096}\mathbf{f}\right) \f$
 *          <br>
 *          5.Permutation: \f$ \mathbf{R}_{8192}\times\left(\left(\mathbf{F}_{16}\right)^{512}\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2\mathbf{D}_{2,4096}\mathbf{f}\right) \f$
 *
 *          In the last step above, we see the permutation operation as
 *          \f$ \mathbf{R}_{8192}\times\left(\left(\mathbf{F}_{16}\right)^{512}\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2\mathbf{D}_{2,4096}\mathbf{f}\right) \f$,
 *          where \f$ \mathbf{R}_{8192} \f$ is the product of several permutation
 *          matrices - that is, \f$ \mathbf{R}_{8192} = \mathbf{P}_{8192}\left(\mathbf{P}_{4096}\right)^2\left(\mathbf{P}_{256} \right)^{32}\f$.
 *          To understand the effect of multiplication of data by 
 *          \f$ \mathbf{R}_{8192} \f$ matrix, we illustrate what happens to the
 *          data when left-multiplied by the \f$ \mathbf{P} \f$ matrices taken
 *          one by one from right to left in the product
 *          \f$ \mathbf{R}_{8192} \f$.
 *          <br><br>
 *          The data vector input to the permutation operation is
 *          \f$ \left(\mathbf{F}_{16}\right)^{512}\left(\mathbf{D}_{16,16}\right)^{32}\left(\mathbf{D}_{16,256}\right)^2\mathbf{D}_{2,4096}\mathbf{f} \f$, 
 *          which we denote as \f$ \mathbf{x} \f$. Assume the data content of 
 *          \f$ \mathbf{x} \f$ is as follows. Note that there are \f$ 8192 \f$ 
 *          data elements.
 *
 *          \image html highRadixDecompositions_permutation_data.svg
 *          \image latex highRadixDecompositions_permutation_data.eps
 *
 *          This data is to be left-multiplied first by the right-most 
 *          permutation matrix, namely 
 *          \f$ \left(\mathbf{P}_{256}\right)^{32} \f$. The effect of this 
 *          multiplication 
 *          \f$ \left(\mathbf{P}_{256}\right)^{32}\times \mathbf{x} \f$ is to 
 *          form \f$ 32 \f$ groups of \f$ 256 \f$ elements each, and to count 
 *          by \f$ 16 \f$s in each group. The resultant data 
 *          \f$ \left(\mathbf{P}_{256}\right)^{32}\mathbf{x} \f$ is shown below.
 *          <br><br>
 *          \image html highRadixDecompositions_permutation_1.svg
 *          \image latex highRadixDecompositions_permutation_1.eps
 *
 *          Then, the above data is to be left-multiplied by the next (going
 *          right to left in the product \f$ \mathbf{R}_{8192} \f$) permutation
 *          matrix \f$ \left(\mathbf{P}_{4096}\right)^2 \f$. The effect of this
 *          multiplication \f$ \left(\mathbf{P}_{4096}\right)^2 \times \left( \left(\mathbf{P}_{256}\right)^{32}\mathbf{x}\right) \f$ is to form \f$ 2 \f$ groups of \f$ 4096 \f$ elements
 *          each, and to count by \f$ 256 \f$s in each group. The resultant data
 *          \f$ \left(\mathbf{P}_{4096}\right)^2\left(\mathbf{P}_{256}\right)^{32}\mathbf{x} \f$ 
 *          is shown below.
 *
 *          \image html highRadixDecompositions_permutation_2.svg
 *          \image latex highRadixDecompositions_permutation_2.eps
 *
 *          Finally, the above data is to be left-multiplied by the next 
 *          permutation matrix \f$ \mathbf{P}_{8192} \f$. The effect of this
 *          multiplication \f$ \mathbf{P}_{8192}\times \left(\left(\mathbf{P}_{4096}\right)^2\left(\mathbf{P}_{256}\right)^{32}\mathbf{x}\right) \f$ is to count by \f$ 4096 \f$s in the entire data
 *          vector. The resultant data
 *          \f$ \mathbf{P}_{8192}\left(\mathbf{P}_{4096}\right)^2\left(\mathbf{P}_{256}\right)^{32}\mathbf{x} \f$ 
 *          is shown below.
 *
 *          \image html highRadixDecompositions_permutation_3.svg
 *          \image latex highRadixDecompositions_permutation_3.eps
 *
 *          Thus, the effect of multiplying \f$ \mathbf{R}_{8192} \f$ matrix and the
 *          \f$ 8192 \f$-point data vector \f$ \mathbf{x} \f$ is to first count
 *          by \f$ 4096 \f$s, then count by \f$ 256 \f$s, then count by 
 *          \f$ 16 \f$s, and finally count by \f$ 1 \f$s. 
 *          <br><br>
 *          Following notes describe how the algorithm outlined above is 
 *          implemented on C7x+MMA.
 *
 *          Each of the decomposition stages can be computed using C7x+MMA in 
 *          two operations:
 *
 *          - matrix multiplication on MMA between a dense matrix formed by 
 *          choosing only the non-zero entries in the sparse \f$ \mathbf{D} \f$
 *          matrix and a dense matrix formed by rearranging the data vector. The
 *          dense matrices corresponding to the \f$ \mathbf{D} \f$ matrices are
 *          prepared a priori by the 
 *          @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen function.
 *          For the data vector, the appropriate rearrangement is accomplished 
 *          using the streaming engine.
 *          - point-wise multiplication on C7x to multiply "left-overs" from
 *          the \f$ \mathbf{D} \f$ matrix. This is required because the dense 
 *          matrix multiplication on MMA does not completely accomplish the 
 *          multiplication by the \f$ \mathbf{D} \f$ matrix. This operation 
 *          piggybacks on the dense matrix multiplication, and hence does not 
 *          consume many additional cycles.
 *
 *          Then, the DFTs' stage is accomplished on MMA as dense matrix 
 *          multiplications. Each \f$ \mathbf{F}_{16} \f$ block of the 
 *          \f$ \left(\mathbf{F}_{16}\right)^N \f$ block diagonal matrix forms 
 *          one of the dense matrices, while data vector is rearranged into
 *          the other dense matrix. Rearrangement of data vector is 
 *          accomplished by the streaming engine and the MMA in transpose mode.
 *
 *          The permutation operation does not take an additional computation
 *          stage. It merely piggybacks to the DFTs' stage, and does not take 
 *          any additional cycles. This is accomplished by virtue of reading
 *          the data for DFTs' stage in the proper sequence using the streaming
 *          engine and storing the data in the proper sequence using the stream
 *          address generator.
 *
 *          The implementatation assumes the input complex data has its real and
 *          imaginary parts interleaved. If the input complex data is 
 *          non-interleaved, the implementation employs an additional loop in
 *          the beginning to first interleave the input data, and an additional
 *          loop in the end to deinterleave the output data.
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the computational
 *                             matrices
 *  @param [out] pY          : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 *  @par Performance Considerations:
 *    For best performance,
 *    - the input and output data buffers are expected to be in L2 memory
 *    - the buffer pointers are assumed to be 64-byte aligned
 *
 */
template <typename dataType, uint32_t MMA_SIZE>
extern FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci(
   FFTLIB_kernelHandle handle,
   void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/*!
 *  @brief This function is the main execution function for the natural
 *         C implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec.
 *
 *  @details None
 *
 *  @param [in]  handle      : Active handle to the kernel
 *  @param [in]  pX          : Pointer to buffer holding the input data
 *  @param [in]  pW          : Pointer to buffer holding the computational
 *                             matrices
 *  @param [out] pY          : Pointer to buffer holding the output data
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */
template <typename dataType, typename accumulateType, uint32_t MMA_SIZE>
extern FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn(
   FFTLIB_kernelHandle handle,
   void *restrict pX,
   const void *restrict pW,
   void *restrict pY);

/*!
 *  @brief   This is a utility function that generates the computational 
 *           matrices (DFT and twiddle factor matrices).
 *
 *  @details Of the data generated by this function, a part is written into
 *           the buffer pointed to by the pointer pW, while the other part 
 *           is written into the @ref FFTLIB_FFT_twData buffer. 
 *           @ref FFTLIB_FFT_twData must be mapped to L1D memory for 
 *           optimal performance. The buffer pointer pW must also be passed
 *           as a parameter to 
 *           @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec function.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *  @param [in]  pW           :  Pointer to buffer holding the computational
 *                               matrices
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices' buffer
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */
template <typename dataType>
extern FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_twGen(
   const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   void *restrict pW,
   const FFTLIB_bufParams1D_t * bufParamsW);

/*!
 *  @brief This function is a utility function that the application calls to
 *         obtain the sizes of data buffers. 
 *
 *  @details This function calculates the sizes of the buffers based on FFT
 *          size and batch size.
 *
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices' buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer 
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */
template <uint32_t dataType>
extern FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_getSizes(
   const FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs * pKerInitArgs,
   FFTLIB_bufParams1D_t * bufParamsX, 
   FFTLIB_bufParams1D_t * bufParamsW, 
   FFTLIB_bufParams1D_t * bufParamsY);

/*!
 * @brief Structure that is reserved for internal use by the kernel
 */
typedef struct
{
   /*! @brief Structure holding initialization parameters                     */
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs initArgs;
   /*! @brief Function pointer to point to the right execution variant between
    *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_cn and 
    *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci.    */
   pFxnFFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec execute;
   /*! @brief Array to store the configuration prepared by 
    *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_ci that 
    *         will be retrieved and used by 
    *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_exec_ci     */
   uint8_t bufPblock[FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_PBLOCK_SIZE];
   /*! @brief Pointer to @ref FFTLIB_FFT_twData buffer that is mapped to 
    *  L1D for performance reasons.                                           */
   uint8_t *pW;
} FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_PrivArgs;

/*!
 *  @brief This function is the initialization function for the natural C
 *         implementation of the kernel. The function declaration conforms
 *         to the declaration of 
 *         @ref FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init.
 *
 *  @details None 
 *
 *  @param [in]  handle       :  Active handle to the kernel
 *  @param [in]  bufParamsX   :  Pointer to the structure containing dimensional
 *                               information of input buffer
 *  @param [in]  bufParamsW   :  Pointer to the structure containing dimensional
 *                               information of computational matrices' buffer
 *  @param [out] bufParamsY   :  Pointer to the structure containing dimensional
 *                               information of ouput buffer 
 *  @param [in]  pKerInitArgs :  Pointer to the structure holding init parameters
 *
 *  @return      Status value indicating success or failure. Refer to @ref FFTLIB_STATUS. 
 *
 */
FFTLIB_STATUS FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_init_cn(
   FFTLIB_kernelHandle handle,
   FFTLIB_bufParams1D_t *bufParamsX,
   FFTLIB_bufParams1D_t *bufParamsW,
   FFTLIB_bufParams1D_t *bufParamsY,
   FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX_InitArgs *pKerInitArgs);

#endif /* FFTLIB_FFT_HIGHRADIXDECOMPOSITIONS_IXX_CXX_OXX_PRIV_H_ */

/* ======================================================================== */
/*  End of file:  FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX.h          */
/* ======================================================================== */

