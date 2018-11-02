#!/bin/tcsh

# draw the simulation box
set minx 0
set miny 0
set minz 0

set maxx [exec grep "Box length x" param.out | awk {{print $NF}}]
set maxy [exec grep "Box length y" param.out | awk {{print $NF}}]
set maxz [exec grep "Box length z" param.out | awk {{print $NF}}]
set cx [expr {$maxx/2.0}]
set cy [expr {$maxy/2.0}]
set cz [expr {$maxz/2.0}]

set delta [exec grep "\(saveCount\)" param.out | awk {{print $NF}}]
set startFile $delta 
set endFile [exec tail -1 "simProg.txt" | awk {{print $1}}]
set incrFile [expr {$delta - 1}]

draw color white

draw line "$minx $miny $minz" "$minx $maxy $minz"
draw line "$minx $miny $minz" "$minx $miny $maxz"

draw line "$maxx $miny $minz" "$maxx $maxy $minz"
draw line "$maxx $miny $minz" "$maxx $miny $maxz"

draw line "$minx $maxy $minz" "$maxx $maxy $minz"
draw line "$minx $maxy $minz" "$minx $maxy $maxz"

draw line "$minx $miny $maxz" "$maxx $miny $maxz"
draw line "$minx $miny $maxz" "$minx $maxy $maxz"

draw line "$maxx $maxy $maxz" "$maxx $maxy $minz"
draw line "$maxx $maxy $maxz" "$minx $maxy $maxz"
draw line "$maxx $maxy $maxz" "$maxx $miny $maxz"

draw line "$minx $miny $minz" "$maxx $miny $minz"
draw line "$minx $miny $minz" "$maxx $miny $minz"

puts "the simulation box is drawn"

for {set x $startFile} {$x < $endFile} {incr x} {	 
	mol addfile ./data/XYZ${x}.xyz step [expr {$x-0}]
	set x [expr {$x+ $incrFile}]
}

#mol new /storage/thejas/stage1/code/data/XYZ1.xyz

puts "the molecule for this problem is 0"

# type H - fluid
# type O - wall

# colorID 0 - blue
# colorID 2 - gray
mol addrep 0
mol modselect 0 0 type H
mol modselect 1 0 type O
mol addrep 0
mol modselect 2 0 type C
mol addrep 0
mol modselect 3 0 type B

puts "the representation is created and the solid and fluid atoms are labelled"

mol modstyle  0 0 Points 3.0
mol modstyle  1 0 Points 3.0
mol modstyle  2 0 Points 3.0
mol modstyle  3 0 Points 3.0

puts "the Points representation is chosen for both solids and fluids"

mol modcolor  0 0 ColorID 4
mol modcolor  1 0 ColorID 1
mol modcolor  2 0 ColorID 0
mol modcolor  3 0 ColorID 2

puts "the solid is red in color and the fluid is blue in color"

display reset view
display projection Orthographic
rotate x by -90

# create a clipping plane
# format : mol clipplane <command> <clipid> <repid> <molid> <value>
#mol clipplane center 0 0 0 "$cx $cy $cz"
#mol clipplane center 1 1 0 "$cx $cy $cz"
#mol clipplane normal 0 0 0 "0.0 1.0 0.0"
#mol clipplane normal 1 1 0 "0.0 1.0 0.0"
#mol clipplane status 0 0 0 1
#mol clipplane status 1 1 0 1
#mol clipplane color  0 0 0 "1 1 1"
#mol clipplane color  1 1 0 "0 0 0"

# read the data file
# for command format : for {initialization} {test} {increment} {task} 

# CPK : Sphere Scale , Sphere Resolution, Bond Radius, Bond Resolution
# mol modstyle  0 0 CPK 0.6 1.0 0.0 1.0
# mol modstyle  1 0 CPK 0.6 1.0 0.0 1.0
