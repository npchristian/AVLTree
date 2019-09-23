This is a database utilizing AVLTree Structure. It reads in a set of numbers (example temperature.csv) and stores them into a database intended to reduce the search complexity to O(n)as a worst case scenario.

In an AVL Tree the height of all branches should differ by no more than 1 level. 

Inserting a number: When inserting. lesser numbers should go left and greater numbers go right of the root and subsequent branches/nodes.
If the insertion causes the tree to become out of balance (left or right branch is more than 1 level greater in height) than the tree reassigns nodes, and rotates and moves data to keep the tree balanced. This property is what allows the tree to be an O(n) search. 

For more information see: https://www.cs.auckland.ac.nz/software/AlgAnim/AVL.html
