#!bin/bash

echo "User Info	->	$(users)" >> info.log
echo "OS Type / Brand	->	$(lsb_release -d)" >> info.log
echo "OS Distribution	->	$(lsb_release -i)" >> info.log
echo "OS version	->	$(lsb_release -r)" >> info.log
echo "Kernel version	->	$(uname -r)" >> info.log
echo "GCC version build	->	$(gcc --version | head -n 1)" >> info.log
echo "Kernel build time	->	$(uname -v)" >> info.log
echo "System arch info	->	$(uname --m)" >> info.log
echo "Info on System mem->	$(free -m)" >> info.log
