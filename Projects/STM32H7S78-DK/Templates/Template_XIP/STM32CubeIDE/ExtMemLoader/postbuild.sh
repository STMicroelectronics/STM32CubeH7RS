#!/bin/bash
# set cubeprog variable check
cubeprog=1
# Getting the project path
projectdir="$(dirname "$(readlink -f "$0")")"
# Check the operating system
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Commands for Linux
    export dest_dir=$STM32_PRG_PATH
    # Commands for Ubuntu to extract STM32_PRG_PATH from /.bashrc
    if [[ -z "${dest_dir}" ]]; then
        dest_dir=$(grep "^export STM32_PRG_PATH=" ~/.bashrc | cut -d'=' -f2)
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # Commands for macOS
    export dest_dir=$STM32_PRG_PATH
elif [[ "$(echo "$OS" | grep -Ei 'win(dows)?(_nt)?')" ]]; then
    # Commands for Windows
    export dest_dir=$STM32_PRG_PATH
else
    echo "Unknown operating system"
fi
# check if STM32Cubeprogrammer env variable exists or not
if [[ -z "${dest_dir}" ]]; then
  echo "warning :: the env variable STM32_PRG_PATH is undefined"
  cubeprog=0
fi

dest_dir=$dest_dir/ExternalLoader
cubeide_cubeprogrammer_path=$1
new_ext=".stldr"
keyword="ExtMemLoader"
keyword_CUBEIDE="Debug"
extension=".ewp"

# check used IDE EWARM or STM32CubeIDE
for j in $(find "$projectdir" -name "*$keyword*$extension" 2>/dev/null); do
	EWARM=true
done
extension=".cproject"
for j in $(find "$projectdir" -name "$extension" 2>/dev/null); do
	STM32CUBE=true
done

if [ "$EWARM" = true ]; then
    echo "EWARM IDE is found!"
    extension=".out"
    for j in $(find "$projectdir" -name "*$keyword*$extension" 2>/dev/null); do
        projectCopy="$j"
    done
	if [ "$cubeprog" = 1 ]; then
		for f in "$projectCopy"; do
			cp "$f" "$dest_dir/$(basename "${f%.*}")$new_ext"
		done
		if [ $? -eq 0 ]; then
			echo "File copied successfully"
		else
			echo "Error: File copy failed please check that the folder exists or you have the right permission to copy the ExtMemLoader"
		fi
	fi
	exit 0	
fi
if [ "$STM32CUBE" = true ]; then
    echo "STM32CUBE IDE is found!"
    extension=".elf"
    for j in $(find "$projectdir" -name "*$keyword*$extension" 2>/dev/null); do
        projectCopy="$j"
    done

	if [ "$cubeprog" = 1 ]; then
	  for f in "$projectCopy"; do
          cp -a "$f" "$dest_dir/$(basename "${f%.*}")$new_ext"
      done
	  if [ $? -eq 0 ]; then
		  echo "File copied successfully under STM32CubeProgrammer."
	  else
		 echo "warning :: File copy failed under STM32CubeProgrammer please check that the folder exists or you have the right permission to copy the ExtMemLoader."
	  fi
	fi
if [ -d "$cubeide_cubeprogrammer_path" ]; then
    for f in "$projectCopy"; do
		cp -a "$f" "$cubeide_cubeprogrammer_path/$(basename "${f%.*}")$new_ext"
    done
    if [ $? -eq 0 ]; then
		echo "File copied successfully under STM32CubeIDE"
	else
		echo "warning :: File copy failed under STM32CubeIDE please check that you have the right permission to copy the ExtMemLoader."
    fi
else
   echo "warning :: File copy failed under STM32CubeIDE please check that the folder exists."
fi
	
    
	exit 0
fi
