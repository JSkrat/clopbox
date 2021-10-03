$fn = 45;
t = 0.01;
clearance = 0.2;
wall = 0.9;


include <3d-stuff/engines.scad>;

module casing() {
    difference() {
        union() {
            linear_extrude(toy_engine_h_below + toy_engine_h_above + wall*2) {
                points = [
                    [7.75, toy_engine_depth/2 + wall + 1.7],
                    [toy_engine_width/2 + wall, 2.75],
                    [toy_engine_width/2 + wall, -2.75],
                    [9, -toy_engine_depth/2 - wall - 2]
                ];
                hull() // so we do not care about the order of points
                polygon([
                    for (x = [-1, 1]) 
                        //for (y = [-1, 1]) 
                            for (p = points) 
                                [p[0] * x, p[1] * 1]
                ]);
            }
        }
        translate([0, 0, toy_engine_h_below + wall])
        toy_engine(false);
        // space for wires
        wire_w = 10;
        wire_d = 2.4;
        wire_h = 1.5;
        wire_diameter = 1.5 + clearance*2;
        // wire hole to the bottom side
        translate([-wire_w/2, -toy_engine_depth/2 - wire_d, 4.1 - wire_h])
        cube([wire_w, wire_d, wire_h]);
        // hole for wires
        hull() {
            for (x = [-1, 1])
                translate([wire_diameter/2*x, -toy_engine_depth/2 - wire_diameter/2 - 0.5, -t])
                cylinder(d = wire_diameter, h = 4.1 - wire_h + t*2);
        }
        // wire hole to the back side
        /*rotate(90, [1, 0, 0])
        hull() {
            for (x = [-1, 1])
                translate([wire_diameter/2*x, 7, 0])
                cylinder(d = wire_diameter, h = toy_engine_depth);
        }*/
        translate([0, 0, toy_engine_h_below + wall + toy_engine_head_h])
        cylinder(r = 9 + clearance, h = toy_engine_h_above - toy_engine_head_h);
    }
}

*difference() {
    casing();
    //mirror([0, 1, 0])
    *translate([-100, 0, 0])
    cube([200, 200, 200]);
    translate([-100, -100, 9.8])
    cube([200, 200, 200]);
}

// parts are hardcoded to be on the same height
// print both halves on outside sides
rotate(90, [1, 0, 0]) {
    translate([0, 10.4, 0])
    difference() {
        casing();
        translate([-100, 0, -t])
        cube([200, 200, 200]);
    }

    translate([25, 10.1, 0])
    rotate(180, [0, 0, 1])
    difference() {
        casing();
        mirror([0, 1, 0])
        translate([-100, 0, -t])
        cube([200, 200, 200]);
    }
}
