# This are real test data generated with the following steps

* Compile, load and run *Trice* test project [https://github.com/rokath/trice/tree/master/test/MDK-ARM_STM32F030R8](https://github.com/rokath/trice/tree/master/test/MDK-ARM_STM32F030R8).
* Then:

```b
trice l -p COM6 -debug | grep "PKG:" | cut -c 4- > tricePackagesGrep.txt
cat tricePackagesGrep.txt | sort | uniq > tricePackagesGrepSortUniq.txt
cat tricePackagesSortUniq.txt | cut -c 6- > tricePackagesSortUniqPlain.txt
cat tricePackagesGrepSortUniqPlain.txt | cut -c 13- > tricePackagesGrepSortUniqPlainNoDescriptor.txt
```

* The file `tricePackagesGrepSortUniqPlainNoDescriptor.txt` contains typical trice messages without package descriptor.
