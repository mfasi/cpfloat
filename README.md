# CPFloat: Custom-Precision Floating-point numbers

CPFloat is a header-only C library for simulating low-precision floating-point number formats in software. The library provides a set of routines for converting the elements of `float` or `double` arrays to lower precision using a variety of rounding modes. The low-precision format (target format) follows a straightforward extension of the IEEE 754 standard, and is assumed to be entirely specified by three parameters:
* a positive integer *p*, which represents the number of digits of precision;
* a positive integer *e*<sub>max</sub>, which represents the maximum supported exponent; and
* a Boolean variable &sigma; set to **true** if subnormal are supported and to **false** otherwise.

The largest values of *p* and *e*<sub>max</sub> that can be used depend on the format in which the converted numbers are to be stored (storage format). A more extensive description of the characteristics of the low-precision formats that can be used, together with more details on the choice of the admissible values of *p*, *e*<sub>max</sub>, and *&sigma;* can be found in [[1]](#ref1).

The library was originally intended as a faster version of the MATLAB function [`chop`](https://github.com/higham/chop) [[2]](#ref2), thus a MEX interface to CPFloat is provided in the `mex/` folder.

## Dependencies

The only (optional) dependency of CPFloat is the header file `pcg_variants.h` from the [C implementation](https://github.com/imneme/pcg-c) of the [PCG Library](https://www.pcg-random.org/), which provides a variety of high-quality pseudo-random number generators. For an in-depth discussion of the algorithms underlying this software, we recommend reading the [paper](https://www.pcg-random.org/paper.html) by [Melissa O'Neill](https://www.cs.hmc.edu/~oneill/) [[3]](#ref3), author of the library. If the file `pcg_variants.h` is not included at compile-time with the `--include` compiler option, then CPFloat relies on the default C pseudo-random number generator.

The PCG Library is free software (see the [Licensing information](#licensing-information) below), and its generators are more efficient, reliable, and flexible than any combination of the functions `srand`,`rand`, and `rand_r` from the C standard library. We see no reason not to use it, and a warning is issued at compile time if the location of `pcg_variant.h` is not specified correctly.

Compiling the MEX interface requires a reasonably recent version of MATLAB or Octave, and testing the interface requires the function [`float_params`](https://github.com/higham/float_params). The unit tests for the C implementation in [`test/cpfloat_test.ts`](test/cpfloat_test.ts) require the [check unit testing framework for C](https://libcheck.github.io/check), including the `checkmk` script.

The code in the `experiments/` folder can be used to replicate the experiments discussed in [[1]](#ref1). The C experiments require the [GNU MPFR library](https://www.mpfr.org/) and the [FloatX library](https://github.com/oprecomp/FloatX). The MATLAB experiments depend on the [`chop`](https://github.com/higham/chop) function and on the [FLOATP_Toolbox](https://gerard-meurant.pagesperso-orange.fr/floatp.zip).

All the dependencies can be installed automatically by issuing
```console
make init
```
If necessary, this Makefile target clones the submodules in the `include/` directory and downloads the latest version of the [FLOATP_Toolbox](https://gerard-meurant.pagesperso-orange.fr/floatp.zip) from the website of the author.

## Installation

CPFloat is a header-only library, and can be used as long as the files in `src/` are in the include path of the compiler. In order to use the PCG Library, one can either:
* specify the path of the file `pcg_variants.h` using the preprocessor option `--include` (currently used in the [Makefile](Makefile), see the variable `CFLAGS` for an example); or
* make sure that `pcg_variants.h` is in the include path and uncomment the preprocessor instruction on line 24 of [`src/cpfloat_definitions.h`](src/cpfloat_definitions.h#L24), that is,
```C
/* #include "pcg_variants.h" */
```

#### MEX interface

The MEX interface can be compiled automatically with either
```console
make mexmat # Compile MEX interface for MATLAB.
```
or
```console
make mexoct # Compile MEX interface for Octave.
```
These two commands compile and autotune the MEX interface in MATLAB and Octave, respectively, by using the functions [`mex/cpfloat_compile.m`](mex/cpfloat_compile.m) and [`mex/cpfloat_autotune.m`](mex/cpfloat_autotune.m).

On a system where the `make` build automation tool is not available, we recommend building the MEX interface by running the script [`cpfloat_compile_nomake.m`](cpfloat_compile_nomake.m) in the `mex/` folder. The script attempts to download the file `pcg_variants.h`  and to compile and auto-tune the MEX interface using the default C compiler. A different compiler can be used by setting the value of the variable `compilerpath` appropriately.

If the PCG Library header file cannot be downloaded and is not already present in the `include/pcg-c/include` folder, then the interface falls back to the pseudo-random number generator in the C standard library. If the compiler does not support OpenMP, only the sequential version of the algorithm will be produced and no auto-tuning will take place.

#### Auto-tuning

CPFloat provides a sequential and a parallel implementation of the rounding functions. Because of some overhead due to the use of OpenMP, using a single thread is typically faster for arrays with few elements, and the library provides a facility to switch between the single-threaded and the multi-threaded variants automatically, depending on the size of the input. The threshold is machine-dependent, and the best value for a given system can be found by invoking
```console
make autotune
```
which compiles the file [`src/cpfloat_autotune.c`](src/cpfloat_autotune.c), runs it, and updates the files [`src/cpfloat_threshold_binary32.h`](src/cpfloat_threshold_binary32.h) and [`src/cpfloat_threshold_binary64.h`](src/cpfloat_threshold_binary64.h).

#### Validation

The `test/` folder contains two sets of test, one for the C library and one for the MEX interface. The unit tests for the C implementation require the `check` library, and can be run with
```console
make ctest
```
which generates the file [`test/cpfloat_test.c`](test/cpfloat_test.c) from [`test/cpfloat_test.ts`](test/cpfloat_test.ts), compiles it, and runs the tests. The Makefile target `coverage` measures the code coverage using GNU `gcov` on the same set of tests.

The MEX interface can be tested by using either
```console
make mtest # Test MEX interface using MATLAB.
```
or
```console
make otest # Test MEX interface using Octave.
```
These two commands run, in MATLAB and Octave respectively,
the function [`test/cpfloat_test.m`](test/cpfloat_test.m), a test set based on the MATLAB function [`test_chop`](https://github.com/higham/chop/blob/master/test_chop.m) with modifications that make it compatible with Octave.

#### Documentation

The documentation of CPFloat can be generated with the command
```console
make docs
```
which relies on [Doxygen](https://www.doxygen.nl/) to format nicely the Javadoc-style comments in the source files. The HTML and LaTeX versions of the documentations are generated in the `doc/html/` and `doc/latex/` folders, respectively.

## Experiments

All the tests in [[1]](#ref1) can be reproduced using the command
```console
make experiments
```
provided that all the relevant dependencies are installed as discussed in the [Dependencies section](#dependencies) above and the MEX interface has been compiled.

## References

<a name="ref1">[1]</a> Massimiliano Fasi and Mantas Mikaitis. [*CPFloat: A C library for emulating low-precision arithmetic*](http://eprints.maths.manchester.ac.uk/2785). MIMS EPrint 2020.22, Manchester Institute for Mathematical Sciences, The University of Manchester, UK, October 2020.

<a name="ref2">[2]</a> Nicholas J. Higham and Srikara Pranesh, [*Simulating Low Precision Floating-Point Arithmetic*](https://doi.org/10.1137/19M1251308), SIAM J. Sci. Comput., 41, C585-C602, 2019.

<a name="ref3">[3]</a> Melissa E. O'Neill, [*PCG: A family of simple fast space-efficient statistically good algorithms for random number generation*](https://www.pcg-random.org/paper.html), Technical report HMC-CS-2014-0905, Harvey Mudd College, Claremont, CA, September 2014.

## Licensing information

CPFloat is distributed under the GNU Lesser General Public License, Version 2.1 or later. The terms of the GNU General Public License, Version 2, and of the Lesser GNU General Public License, Version 2.1, are in the [LICENSES](LICENSES) folder. Please contact us if you would like to use CPFloat in an open source project distributed under the terms of a license that is incompatible with the GNU LGPL. We might be able to relicense the software for you.

The PCG Library is distributed under the terms of either the [Apache License, Version 2.0](https://raw.githubusercontent.com/imneme/pcg-c/master/LICENSE-APACHE.txt) or the [Expat License](https://raw.githubusercontent.com/imneme/pcg-c/master/LICENSE-MIT.txt), at the option of the user.

The MATLAB function `float_params` is distributed under the terms of the [BSD 2-Clause "Simplified" License](https://raw.githubusercontent.com/higham/float_params/master/license.txt).

The GNU MPFR Library is distributed under the terms of the [GNU Lesser General Public License (GNU Lesser GPL), Version 3](https://www.gnu.org/licenses/lgpl-3.0.html) or later ([Version 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) or later for MPFR versions until 2.4.x).

The FloatX Library is distributed under the terms of the [Apache License, Version 2.0](https://raw.githubusercontent.com/oprecomp/FloatX/master/LICENSE).

The MATLAB function `chop` is distributed under the terms of the [BSD 2-Clause "Simplified" License](https://raw.githubusercontent.com/higham/chop/master/license.txt).

The FLOATP Toolbox is distributed under an informal license and without any warranty.
