TEMPLATE = subdirs

# Build order: SIMD libraries first, then main library
SUBDIRS += \
    xsimd_sse2 \
    xsimd_avx2 \
    xsimd

# Main library depends on SIMD libraries
xsimd.depends = xsimd_sse2 xsimd_avx2
