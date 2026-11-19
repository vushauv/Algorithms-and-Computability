# Third-party code

`boost/` holds the headers of Boost.Multiprecision and Boost.Config, copied
unchanged from Boost 1.83 (https://www.boost.org/), under the Boost Software
License 1.0 (`boost/LICENSE_1_0.txt`). The program uses `cpp_int`, the
integer of arbitrary size, in standalone mode (`-DBOOST_MP_STANDALONE`); it
is header-only, so nothing is linked and nothing has to be installed.
