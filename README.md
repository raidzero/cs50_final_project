cs50_final_project
==================

A hex diff tool

compile with Make 

USAGE: hdiff FILE1 FILE2 [-as|bs ASCIISEARCH|BINARYSEARCH] [ -c COLUMNS]

Will print files side by side, differences shown in red. 
Auto-detects terminal size if COLUMNS is not passed. 
Optional Searched word shown in green.
Identical bytes shown in white. 
