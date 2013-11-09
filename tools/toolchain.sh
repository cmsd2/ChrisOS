#! /bin/bash

#
# Copyright (c) 2009 Martin Decky
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# - The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

GMP_MAIN=<<EOF
#define GCC_GMP_VERSION_NUM(a, b, c) \
	(((a) << 16L) | ((b) << 8) | (c))

#define GCC_GMP_VERSION \
	GCC_GMP_VERSION_NUM(__GNU_MP_VERSION, __GNU_MP_VERSION_MINOR, __GNU_MP_VERSION_PATCHLEVEL)

#if GCC_GMP_VERSION < GCC_GMP_VERSION_NUM(4,3,2)
	choke me
#endif
EOF

MPFR_MAIN=<<EOF
#if MPFR_VERSION < MPFR_VERSION_NUM(2, 4, 2)
choke me
	#endif
EOF

MPC_MAIN=<<EOF
#if MPC_VERSION < MPC_VERSION_NUM(0, 8, 1)
	choke me
#endif
EOF

BINUTILS_VERSION="2.23.2"
BINUTILS_RELEASE=""
GCC_VERSION="4.8.2"
GDB_VERSION="7.6"
MPC_VERSION="1.0.1"
MPFR_VERSION="3.1.2"
GMP_VERSION="5.1.3"
ICONV_VERSION="1.14"

BASEDIR="`pwd`"
BINUTILS="binutils-${BINUTILS_VERSION}${BINUTILS_RELEASE}.tar.bz2"
GCC="gcc-${GCC_VERSION}.tar.bz2"
GDB="gdb-${GDB_VERSION}.tar.bz2"
MPC="mpc-${MPC_VERSION}.tar.gz"
MPFR="mpfr-${MPFR_VERSION}.tar.bz2"
GMP="gmp-${GMP_VERSION}.tar.bz2"
ICONV="libiconv-${ICONV_VERSION}.tar.gz"

#
# Check if the library described in the argument
# exists and has acceptable version.
#
check_dependency() {
	DEPENDENCY="$1"
	HEADER="$2"
	BODY="$3"
	
	FNAME="/tmp/conftest-$$"
	
	echo "#include ${HEADER}" > "${FNAME}.c"
	echo >> "${FNAME}.c"
	echo "int main()" >> "${FNAME}.c"
	echo "{" >> "${FNAME}.c"
	echo "${BODY}" >> "${FNAME}.c"
	echo "	return 0;" >> "${FNAME}.c"
	echo "}" >> "${FNAME}.c"
	
	cc -c -o "${FNAME}.o" "${FNAME}.c" 2> "${FNAME}.log"
	RC="$?"
	
	if [ "$RC" -ne "0" ] ; then
		echo " ${DEPENDENCY} not found, too old or compiler error."
		echo " Please recheck manually the source file \"${FNAME}.c\"."
		echo " The compilation of the toolchain is probably going to fail,"
		echo " you have been warned."
		echo
		echo " ===== Compiler output ====="
		cat "${FNAME}.log"
		echo " ==========================="
		echo
	else
		echo " ${DEPENDENCY} found"
		rm -f "${FNAME}.log" "${FNAME}.o" "${FNAME}.c"
	fi
}

check_dependecies() {
	echo ">>> Basic dependency check"
	check_dependency "GMP" "<gmp.h>" "${GMP_MAIN}"
	check_dependency "MPFR" "<mpfr.h>" "${MPFR_MAIN}"
	check_dependency "MPC" "<mpc.h>" "${MPC_MAIN}"
	echo
}

check_error() {
	if [ "$1" -ne "0" ]; then
		echo
		echo "Script failed: $2"
		
		exit 1
	fi
}

check_md5() {
	FILE="$1"
	SUM="$2"
	
	COMPUTED="`md5sum "${FILE}" | cut -d' ' -f1`"
	if [ "${SUM}" != "${COMPUTED}" ] ; then
		echo
		echo "Checksum of ${FILE} does not match."
		
		exit 2
	fi
}

check_sha1() {
	FILE="$1"
	SUM="$2"
	
	COMPUTED="`shasum "${FILE}" | cut -d' ' -f1`"
	if [ "${SUM}" != "${COMPUTED}" ] ; then
		echo
		echo "Checksum of ${FILE} does not match."
		
		exit 2
	fi
}

check_gpg() {
	FILE="$1"
	SIGFILE="$2"
	KEYID="$3"
	
	MESSAGE="`gpg --verify "${SIGFILE}" 2>&1`"

	check_error $? "Error verifying gpg signature ${FILE}."

	COMPUTED="`echo "$MESSAGE" | grep 'key ID' | sed 's/.*key ID \(\w*\)/\1/'`"

	if [ "${KEYID}" != "${COMPUTED}" ] ; then
		echo
		echo "Key id of key for ${SIGFILE} does not match."
		
		exit 2
	fi
}

show_usage() {
	echo "Cross-compiler toolchain build script"
	echo
	echo "Syntax:"
	echo " $0 <platform>"
	echo
	echo "Possible target platforms are:"
	echo " prepare    just fetch deps"
	echo " amd64      AMD64 (x86-64, x64)"
	echo " arm32      ARM"
	echo " ia32       IA-32 (x86, i386)"
	echo " ia64       IA-64 (Itanium)"
	echo " mips32     MIPS little-endian 32b"
	echo " mips32eb   MIPS big-endian 32b"
	echo " mips64     MIPS little-endian 64b"
	echo " ppc32      32-bit PowerPC"
	echo " ppc64      64-bit PowerPC"
	echo " sparc64    SPARC V9"
	echo " all        build all targets"
	echo " parallel   same as 'all', but all in parallel"
	echo " 2-way      same as 'all', but 2-way parallel"
	echo
	echo "The toolchain will be installed to the directory specified by"
	echo "the CROSS_PREFIX environment variable. If the variable is not"
	echo "defined, /usr/local/cross will be used by default."
	echo
	
	exit 3
}

change_title() {
	echo -en "\e]0;$1\a"
}

show_countdown() {
	TM="$1"
	
	if [ "${TM}" -eq 0 ] ; then
		echo
		return 0
	fi
	
	echo -n "${TM} "
	change_title "${TM}"
	sleep 1
	
	TM="`expr "${TM}" - 1`"
	show_countdown "${TM}"
}

show_dependencies() {
	echo "IMPORTANT NOTICE:"
	echo
	echo "For a successful compilation and use of the cross-compiler"
	echo "toolchain you need at least the following dependencies."
	echo
	echo "Please make sure that the dependencies are present in your"
	echo "system. Otherwise the compilation process might fail after"
	echo "a few seconds or minutes."
	echo
	echo " - SED, AWK, Flex, Bison, gzip, bzip2, Bourne Shell"
	echo " - gettext, zlib, Texinfo, libelf, libgomp"
	echo " - GNU Multiple Precision Library (GMP)"
	echo " - GNU Make"
	echo " - GNU tar"
	echo " - GNU Coreutils"
	echo " - GNU Sharutils"
	echo " - MPFR"
	echo " - MPC"
	echo " - Parma Polyhedra Library (PPL)"
	echo " - ClooG-PPL"
	echo " - native C compiler, assembler and linker"
	echo " - native C library with headers"
	echo
}

download_fetch() {
	SOURCE="$1"
	FILE="$2"
	METHOD="$3"
	CHECKSUM="$4"
	EXTRA="$5"
	
	if [ ! -f "${FILE}" ]; then
		change_title "Downloading ${FILE}"
		wget -c "${SOURCE}${FILE}"
		check_error $? "Error downloading ${FILE}."
	fi
	
	if [ "$METHOD" = "MD5" ]; then
		check_md5 "${FILE}" "${CHECKSUM}"
	elif [ "$METHOD" = "SHA1" ]; then
		check_sha1 "${FILE}" "${CHECKSUM}"
	elif [ "$METHOD" = "GPG" ]; then
		wget -c "${SOURCE}${CHECKSUM}"
		check_error $? "Error downloading signature ${CHECKSUM}."
		check_gpg "${FILE}" "${CHECKSUM}" "${EXTRA}"
	fi
}

source_check() {
	FILE="$1"
	
	if [ ! -f "${FILE}" ]; then
		echo
		echo "File ${FILE} not found."
		
		exit 4
	fi
}

cleanup_dir() {
	DIR="$1"
	
	if [ -d "${DIR}" ]; then
		change_title "Removing ${DIR}"
		echo " >>> Removing ${DIR}"
		rm -fr "${DIR}"
	fi
}

create_dir() {
	DIR="$1"
	DESC="$2"
	
	change_title "Creating ${DESC}"
	echo ">>> Creating ${DESC}"
	
	mkdir -p "${DIR}"
	test -d "${DIR}"
	check_error $? "Unable to create ${DIR}."
}

unpack_tarball() {
	FILE="$1"
	DESC="$2"
	
	change_title "Unpacking ${DESC}"
	echo " >>> Unpacking ${DESC}"
	
	tar -xjf "${FILE}"
	check_error $? "Error unpacking ${DESC}."
}

gpg_recv_key() {
	KEYID="$1"
	FINGERPRINT="$2"

	gpg --recv-keys "${KEYID}"
	check_error $? "Error fetching key ${KEYID}"

	MESSAGE="`gpg --fingerprint "${KEYID}" 2>&1`"
	check_error $? "Error fingerprinting key ${KEYID}"

	COMPUTED="`echo "${MESSAGE}" | grep 'Key fingerprint' | sed 's/.*Key fingerprint = \(.*\)$/\1/'`"
	if [ "$COMPUTED" != "$FINGERPRINT" ]; then
		echo
		echo "Fingerprint of key ${KEYID} did not match"
		echo "Expected ${FINGERPRINT}"
		echo "Got      ${COMPUTED}"
		exit 2
	fi
}

prepare() {
	show_dependencies
	#check_dependecies
	#show_countdown 10
	
	BINUTILS_SOURCE="ftp://ftp.gnu.org/gnu/binutils/"
	GCC_SOURCE="ftp://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/"
	GDB_SOURCE="ftp://ftp.gnu.org/gnu/gdb/"
	GMP_SOURCE="ftp://ftp.gnu.org/gnu/gmp/"
	MPFR_SOURCE="ftp://ftp.gnu.org/gnu/mpfr/"
	MPC_SOURCE="ftp://ftp.gnu.org/gnu/mpc/"
	ICONV_SOURCE="ftp://ftp.gnu.org/gnu/libiconv/"

	gpg_recv_key "98C3739D" "07F3 DBBE CC1A 3960 5078  094D 980C 1976 98C3 739D"
	gpg_recv_key "BE216115" "4B02 6187 5C03 D6B1 2E31  7666 09DF 2DC9 BE21 6115"
	gpg_recv_key "4AE55E93" "EAF1 C276 A747 E9ED 8621  0CBA C312 6D3B 4AE5 5E93"
	gpg_recv_key "FC26A641" "1397 5A70 E63C 361C 73AE  69EF 6EEB 81F8 981C 74C7"
	gpg_recv_key "FF325CF3" "F40A DB90 2B24 264A A42E  50BF 92ED B04B FF32 5CF3"
	gpg_recv_key "765C61E3" "AD17 A21E F8AE D8F1 CC02  DBD9 F7D5 C9BF 765C 61E3"
	gpg_recv_key "28C67298" "343C 2FF0 FBEE 5EC2 EDBE  F399 F359 9FF8 28C6 7298"
	gpg_recv_key "C3C45C06" "33C2 35A3 4C46 AA3F FB29  3709 A328 C3A2 C3C4 5C06"
	gpg_recv_key "F059B1D1" "1736 90D4 963E 5FC4 6917  7FA7 C71A 4C65 F059 B1D1"
	
	download_fetch "${BINUTILS_SOURCE}" "${BINUTILS}" "GPG" "${BINUTILS}.sig" "4AE55E93"
	download_fetch "${GCC_SOURCE}" "${GCC}" "GPG" "${GCC}.sig" "C3C45C06"
	download_fetch "${GDB_SOURCE}" "${GDB}" "GPG" "${GDB}.sig" "FF325CF3"
	download_fetch "${MPC_SOURCE}" "${MPC}" "GPG" "${MPC}.sig" "765C61E3"
	download_fetch "${MPFR_SOURCE}" "${MPFR}" "GPG" "${MPFR}.sig" "98C3739D"
	download_fetch "${GMP_SOURCE}" "${GMP}" "GPG" "${GMP}.sig" "28C67298"
	download_fetch "${ICONV_SOURCE}" "${ICONV}" "GPG" "${ICONV}.sig" "F059B1D1"
}

build_target() {
	PLATFORM="$1"
	TARGET="$2"
	
	WORKDIR="${BASEDIR}/${PLATFORM}"
	BINUTILSDIR="${WORKDIR}/binutils-${BINUTILS_VERSION}"
	GCCDIR="${WORKDIR}/gcc-${GCC_VERSION}"
	OBJDIR="${WORKDIR}/gcc-obj"
	GDBDIR="${WORKDIR}/gdb-${GDB_VERSION}"
	MPCDIR="${WORKDIR}/mpc-${MPC_VERSION}"
	MPFRDIR="${WORKDIR}/mpfr-${MPFR_VERSION}"
	GMPDIR="${WORKDIR}/gmp-${GMP_VERSION}"
	ICONVDIR="${WORKDIR}/libiconv-${ICONV_VERSION}"
	
	if [ -z "${CROSS_PREFIX}" ] ; then
		CROSS_PREFIX="/usr/local/cross"
	fi
	
	PREFIX="${CROSS_PREFIX}/${PLATFORM}"
	
	echo ">>> Downloading tarballs"
	source_check "${BASEDIR}/${BINUTILS}"
	source_check "${BASEDIR}/${GCC}"
	source_check "${BASEDIR}/${GDB}"
	
	echo ">>> Removing previous content"
	cleanup_dir "${PREFIX}"
	cleanup_dir "${WORKDIR}"
	
	create_dir "${PREFIX}" "destination directory"
	create_dir "${OBJDIR}" "GCC object directory"
	
	echo ">>> Unpacking tarballs"
	cd "${WORKDIR}"
	check_error $? "Change directory failed."
	
	unpack_tarball "${BASEDIR}/${BINUTILS}" "binutils"
	unpack_tarball "${BASEDIR}/${GCC}" "GCC"
	unpack_tarball "${BASEDIR}/${GDB}" "GDB"
	unpack_tarball "${BASEDIR}/${MPC}" "MPC"
	unpack_tarball "${BASEDIR}/${MPFR}" "MPFR"
	unpack_tarball "${BASEDIR}/${GMP}" "GMP"
	unpack_tarball "${BASEDIR}/${ICONV}" "ICONV"
	
	ln -sf "${MPCDIR}" "${GCCDIR}/mpc"
	ln -sf "${MPFRDIR}" "${GCCDIR}/mpfr"
	ln -sf "${GMPDIR}" "${GCCDIR}/gmp"
	ln -sf "${ICONVDIR}" "${GCCDIR}/libiconv"

	echo ">>> Processing binutils (${PLATFORM})"
	cd "${BINUTILSDIR}"
	check_error $? "Change directory failed."
	
	change_title "binutils: configure (${PLATFORM})"
	CFLAGS=-Wno-error ./configure "--target=${TARGET}" "--prefix=${PREFIX}" "--program-prefix=${TARGET}-" --disable-nls --disable-werror
	check_error $? "Error configuring binutils."
	
	change_title "binutils: make (${PLATFORM})"
	make all install
	check_error $? "Error compiling/installing binutils."
	
	echo ">>> Processing GCC (${PLATFORM})"
	cd "${OBJDIR}"
	check_error $? "Change directory failed."
	
	change_title "GCC: configure (${PLATFORM})"
	"${GCCDIR}/configure" "--target=${TARGET}" "--prefix=${PREFIX}" "--program-prefix=${TARGET}-" --with-gnu-as --with-gnu-ld --disable-nls --disable-threads --enable-languages=c,objc,c++,obj-c++ --disable-multilib --disable-libgcj --without-headers --disable-shared --enable-lto --disable-werror
	check_error $? "Error configuring GCC."
	
	change_title "GCC: make (${PLATFORM})"
	PATH="${PATH}:${PREFIX}/bin" make all-gcc all-target-libgcc install-gcc install-target-libgcc
	check_error $? "Error compiling/installing GCC."
	
	echo ">>> Processing GDB (${PLATFORM})"
	cd "${GDBDIR}"
	check_error $? "Change directory failed."
	
	change_title "GDB: configure (${PLATFORM})"
	./configure "--target=${TARGET}" "--prefix=${PREFIX}" "--program-prefix=${TARGET}-" "--disable-werror"
	check_error $? "Error configuring GDB."
	
	change_title "GDB: make (${PLATFORM})"
	make all install
	check_error $? "Error compiling/installing GDB."
	
	cd "${BASEDIR}"
	check_error $? "Change directory failed."
	
	echo ">>> Cleaning up"
	cleanup_dir "${WORKDIR}"
	
	echo
	echo ">>> Cross-compiler for ${TARGET} installed."
}

if [ "$#" -lt "1" ]; then
	show_usage
fi

case "$1" in
	"prepare")
		prepare
		;;	
	"amd64")
		#prepare
		build_target "amd64" "x86_64-elf"
		;;
	"arm32")
		prepare
		build_target "arm32" "arm-linux-gnueabi"
		;;
	"ia32")
		#prepare
		build_target "ia32" "i686-pc-elf"
		;;
	"ia64")
		prepare
		build_target "ia64" "ia64-pc-linux-gnu"
		;;
	"mips32")
		prepare
		build_target "mips32" "mipsel-linux-gnu"
		;;
	"mips32eb")
		prepare
		build_target "mips32eb" "mips-linux-gnu"
		;;
	"mips64")
		prepare
		build_target "mips64" "mips64el-linux-gnu"
		;;
	"ppc32")
		prepare
		build_target "ppc32" "ppc-linux-gnu"
		;;
	"ppc64")
		prepare
		build_target "ppc64" "ppc64-linux-gnu"
		;;
	"sparc64")
		prepare
		build_target "sparc64" "sparc64-linux-gnu"
		;;
	"all")
		prepare
		build_target "amd64" "amd64-linux-gnu"
		build_target "arm32" "arm-linux-gnueabi"
		build_target "ia32" "i686-pc-linux-gnu"
		build_target "ia64" "ia64-pc-linux-gnu"
		build_target "mips32" "mipsel-linux-gnu"
		build_target "mips32eb" "mips-linux-gnu"
		build_target "mips64" "mips64el-linux-gnu"
		build_target "ppc32" "ppc-linux-gnu"
		build_target "ppc64" "ppc64-linux-gnu"
		build_target "sparc64" "sparc64-linux-gnu"
		;;
	"parallel")
		prepare
		build_target "amd64" "amd64-linux-gnu" &
		build_target "arm32" "arm-linux-gnueabi" &
		build_target "ia32" "i686-pc-linux-gnu" &
		build_target "ia64" "ia64-pc-linux-gnu" &
		build_target "mips32" "mipsel-linux-gnu" &
		build_target "mips32eb" "mips-linux-gnu" &
		build_target "mips64" "mips64el-linux-gnu" &
		build_target "ppc32" "ppc-linux-gnu" &
		build_target "ppc64" "ppc64-linux-gnu" &
		build_target "sparc64" "sparc64-linux-gnu" &
		wait
		;;
	"2-way")
		prepare
		build_target "amd64" "amd64-linux-gnu" &
		build_target "arm32" "arm-linux-gnueabi" &
		wait
		
		build_target "ia32" "i686-pc-linux-gnu" &
		build_target "ia64" "ia64-pc-linux-gnu" &
		wait
		
		build_target "mips32" "mipsel-linux-gnu" &
		build_target "mips32eb" "mips-linux-gnu" &
		wait
		
		build_target "mips64" "mips64el-linux-gnu" &
		build_target "ppc32" "ppc-linux-gnu" &
		wait
		
		build_target "ppc64" "ppc64-linux-gnu" &
		build_target "sparc64" "sparc64-linux-gnu" &
		wait
		;;
	*)
		show_usage
		;;
esac
