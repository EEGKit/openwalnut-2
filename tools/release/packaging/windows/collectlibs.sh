#!/bin/bash

# Where to copy external dependencies? Relative to current dir: bin
target=libExt
mkdir -p "$target"

# Extend PATH to contain "bin" to allow ldd to find libOpenWalnut and stuff
# NOTE: leave orig path at the beginning to allow multiple runs
PATH="$PATH:./lib/:./bin/:./$target/"
export PATH

###############################################################################
# Some files are not yet copied to libExt
###############################################################################

# Although linked dynamically, these files where not found:
# Nifti
echo "Copy NiFTI"
cp /nifticlib-2.0.0/znzlib/libznz.dll "$target"
cp /nifticlib-2.0.0/niftilib/libniftiio.dll "$target"

# Plugins. Not dynamically linked. Loaded on startup. Copy: 
# OSG Plugins
echo "Copy OSG Plugins"
cp -r /mingw64/bin/osgPlugins-* "$target"

# Qt Plugins
echo "Copy Qt Plugins"
cp -r /mingw64/share/qt5/plugins/platforms "bin/"


###############################################################################
# Here is the magic:
###############################################################################

# Collect a list of DLL and Exe we build.
linkables=$(find ./lib -iname "*.dll" -or -iname "*.exe" && find ./bin -iname "*.dll" -or -iname "*.exe")

# Fill all DLLs into this var:
allDeps=""

# Collect a list of files where deps could not be resolved.
warnFiles=""

# For each file which might have dependencies:
for linkable in $linkables
do
	echo "Found file with potential dependencies: $linkable"
	# Get the dependencies, filter out unknowns and Windows DLL
	winDeps=$(ldd "$linkable" | awk '{print $3;}' | grep -i "/c/Windows")
	echo " * INFO: Windows Deps: $winDeps"
	unresolvedDeps=$(ldd "$linkable" | awk '{print $3;}' | grep -i "\?")
	if [ -n "$unresolvedDeps" ]; then
		echo " * WARN: Unresolved Deps!"
		warnFiles="$warnFiles $linkable"
	fi
	localDeps=$(ldd "$linkable" | awk '{print $3;}' | grep -iv "/c/Windows" | grep -iv "\?")
	if [ -n "$localDeps" ]; then
		echo " * INFO: Dependencies found: $localDeps"
		allDeps="$allDeps $localDeps"
	fi
	echo ""
done

###############################################################################
# Copy and finalize
###############################################################################

echo "DONE!"
if [ -n "$warnFiles" ]; then
	echo "WARN: found files with unresolved dependencies. Here is a list:"
	echo " * $warnFiles"
	echo ""
fi

echo "Found the following dependencies. Copying them to $target."
# NOTE: might still be some doubles ...
allDeps=$(echo "$allDeps" | sort | uniq)
echo $allDeps
# CP complains about doubled entries
cp $allDeps "$target"

