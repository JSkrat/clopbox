$fn = 18;
// value for difference cuts
d = 0.005;
tolerance = 0.1;
b_tolerance = 0.3;
layer = 0.25;
wall = 2;
corner_r = 4;

cav_x = 123;
cav_y = 101;
cav_z_lower = 22.3;
cav_z_upper = 23;

module bolt_25() {
    color("silver") {
        cylinder(h = 1.8, d = 4.8);
        mirror([0, 0, 1])
        cylinder(h = 10, d = 1.5 + tolerance*2);
    }
}

module bolt_25_16() {
    color("silver") {
        cylinder(h = 1.8, d = 4.8);
        mirror([0, 0, 1])
        cylinder(h = 16, d = 1.5 + tolerance*2);
    }
}

module bolt_25_h() {
    color("#DE8") {
        mirror([0, 0, 1]) {
            cylinder(d1 = 4.8, d2 = 2.8, h = 1.4);
            translate([0, 0, 1.4])
            cylinder(d1 = 2.8, d2 = 2.0, h = 1.11);
            cylinder(h = 10, d = 1.5 + tolerance*2);        
        }
    }
}

module corner_cut(r, h) {
    difference() {
        translate([-r, -r, -d])
        cube([r*2, r*2, h+d*2]);
        for (x = [-r, r], y = [-r, r]) {
            translate([x, y, -d*2])
            cylinder(r = r, h = h + d*4);
        }
    }
}

module conn() {
    color("#2C6") {
        cube([7.8, 7.3, 9.5]);
        translate([0, -3.2, 9.5])
        cube([7.8, 11.5, 9]);
    }
    color("silver") {
        mirror([0, 0, 1]) {
            translate([1.9, 3.8, 0])
            cube([0.8, 0.8, 3.5]);
            translate([7.8-2.3-0.4, 3.8, 0])
            cube([0.8, 0.8, 3.5]);
        }
    }
}

module big_cap() {
    color("#080")
    cylinder(d = 10.1, h = 21);
}

module diode() {
    color("#333")
    cube([7.0, 6.0, 2.4]);
    color("silver")
    translate([-0.45, 1.5, 0])
    cube([7.9, 3, 1.4]);

}

module pcb(positive) {
    bolt_pos_x = [0.7+1.65, 118+1.65];
    bolt_pos_y = [0.9+1.65, 96.0+1.65];
    bolt_pos_z = [6, 0];
    pcb_h = 1;
    conn_h = 18.3+5;
    translate([0, 0, conn_h])
    if (! positive) {
        // pcb itself
        color("#0A2")
        difference() {
            cube([122, 100, pcb_h]);
            for (x = bolt_pos_x, y = bolt_pos_y) {
                translate([x, y, -d])
                cylinder(d = 3.3, h = pcb_h+d*2);
            }
        }
        translate([0, 0, pcb_h]) {
            // usb
            color("silver")
            translate([0, 67.6-12.1, 0])
            cube([16.5, 12.1, 11]);
            color("white")
            translate([0, 67.6-12.1, 0])
            mirror([1, 0, 0])
            cube([35, 12.1, 11]);
            // power
            color("#333")
            translate([0, 20, 0])
            cube([14, 9, 11.4]);
            color("#333")
            rotate(-90, [0, 1, 0])
            translate([13.3-11.2/2, 24.5, 0])
            cylinder(d = 11.2 + b_tolerance*2, h = 35);
            // bolts
            for (x = bolt_pos_x, y = bolt_pos_y) {
                if (y == bolt_pos_y[0]) {
                    translate([x, y, bolt_pos_z[0]])
                    bolt_25_16();
                    color([1, 1, 1, 0.5])
                    translate([x, y, 0])
                    cylinder(h = 6, d = 3.3);
                } else {
                    translate([x, y, bolt_pos_z[1]])
                    bolt_25();
                }
            }
            // big caps
            minkowski() {
                union() {
                    translate([22.7, 13.5, 0])
                    big_cap();
                    translate([22.7, 25.2, 0])
                    big_cap();
                    // diode
                    translate([14.4, 0.6, 0])
                    diode();
                }
                sphere(r = b_tolerance, $fn = 9);
            }
        }
        // connectors
        minkowski() {
            for (x = [28.7: 11.35714: 116]) {
                mirror([0, 0, 1])
                translate([x, 5.4, 0])
                conn();
            }
            sphere(r = b_tolerance, $fn=9);
        }
        color([1, 1, 1, 0.5])
        translate([20, -10, -conn_h-d])
        cube([87+9.2, 25 + 10, conn_h+d + d]);
        /*translate([122-6, 0, -1])
        cube([6, 12, 1]);*/
    } else {
        // holders
        for (x = bolt_pos_x, y = bolt_pos_y) {
            color("#333")
            translate([x, y, -d])
            cylinder(d = 3.3 - tolerance*2, h = pcb_h + d*2);
            color("#333")
            translate([x, y, 0])
            mirror([0, 0, 1])
            cylinder(d = 9, h = conn_h);
        }
    }
}

module side(d_fit) {
    w = cav_y + wall*2 + d*2;
    h = cav_z_lower + wall + d*2;
    w_bolt = 10;
    r = 2;
    offset = w*0.29;
    minkowski() {
        union() {
            difference() {
                square([w, h]);
                for (y = [0, offset, w - offset, w]) {
                    translate([y, 0])
                    circle(r = cav_z_lower*0.85);
                }
                square([w, r+1]);
            }
            translate([w/2 - w_bolt/2, r+1])
            square([w_bolt, h - r*2 - 1]);
        }
        circle(r = r + d_fit);
    }
}

module screw_pillar_vertical(screw_d, screw_l, screw_r) {
    cylinder(d = screw_d, h = screw_l);
    translate([0, 0, screw_l])
    scale([1, 1, screw_r/screw_d*2])
    sphere(d = screw_d);
}

module screw_pillar() {
    screw_d = 10;
    screw_l = 10;
    screw_r = 2;
    translate([0, 0, screw_d/2]) {
        rotate(90, [0, 1, 0]) {
            screw_pillar_vertical(screw_d, screw_l, screw_r);
        }
        translate([0, 0, -screw_d/2])
        linear_extrude(screw_d/2) {
            projection(cut = false) {
                rotate(90, [0, 1, 0]) {
                    screw_pillar_vertical(screw_d, screw_l, screw_r);
                }                
            }
        }
    }
}

module box_lower(positive, demo) {
    padding = 0.5;
    d_fit = (positive)? tolerance: -tolerance;
    difference() {
        union() {
            difference() {
                translate([-wall + d_fit, 0, 0])
                color("#333")
                cube([cav_x + wall*2 + wall*2 - d_fit*2, cav_y + wall*2, cav_z_lower + wall]);
                // main cavity
                color("#333")
                translate([wall, wall, wall])
                cube([cav_x, cav_y, cav_z_lower + tolerance]);
            }
            // walls for the connectors
            color("#333")
            cube([cav_x + wall*2, 27.5 + wall, cav_z_lower - 2.5 - 1]);
            // pcb holders
            translate([wall+padding, wall+padding, 0])
            pcb(true);
            // for screws
            translate([wall, (cav_y + wall*2)/2, 0])
            screw_pillar();
            translate([wall + cav_x, (cav_y + wall*2)/2, 0])
            mirror([1, 0, 0])
            screw_pillar();
        }
        translate([wall+padding, wall+padding, 0])
        pcb(false);
        // slots for cap
        for (y = [-d, cav_y + wall*1.5 -d_fit]) {
            translate([-d - wall, y, cav_z_lower + wall/2 - d_fit*0])
            cube([d*2 + cav_x + wall*4, wall/2 + d_fit + d, wall/2 + d_fit*0 + d]);
        }
        // cuts for cap slides
        for (x = [-wall-d+d_fit, cav_x + wall*2 + d - d_fit]) {
            translate([x, -d, -d])
            rotate(90, [0, 0, 1])
            rotate(90, [1, 0, 0])
            linear_extrude(wall) side(d_fit);
        }
        // holes for screws
        translate([-wall, cav_y/2 + wall, wall+4])
        rotate(-90, [0, 1, 0])
        bolt_25_h();
        translate([cav_x + wall*3, cav_y/2 + wall, wall+4])
        rotate(90, [0, 1, 0])
        bolt_25_h();
        // hanging thingy
        translate([18 - d_fit, -d, cav_z_lower])
        cube([105 + d_fit*2, wall +d*2, wall + d]);
    }
    if (demo || ! positive) {
        translate([wall+padding, wall+padding, 0])
        pcb(false);
        translate([-wall, cav_y/2 + wall, wall+4])
        rotate(-90, [0, 1, 0])
        bolt_25_h();
        translate([cav_x + wall*3, cav_y/2 + wall, wall+4])
        rotate(90, [0, 1, 0])
        bolt_25_h();
    }
}

module cap() {
    c = [0.25, 0.18, 0.25, 0.7];
    difference() {
        color(c)
        translate([-wall+d, d, d])
        cube([cav_x + wall*4-d*2, cav_y + wall*2-d*2, cav_z_lower + cav_z_upper + wall*2 - d]);
        color(c)
        translate([-d, wall-d, -d])
        cube([cav_x + wall*2 +d*2, cav_y+d*2, cav_z_lower + cav_z_upper + d]);
        box_lower(false, false);
    }
}

module lower_cap() {
    difference() {
        cube([cav_x + wall*2, cav_y + wall*2, wall]);
        translate([0, 0, -d])
        box_lower(false, false);
    }
}

module bracket(demo) {
    difference() {
        translate([0, 0, 24.3])
        union() {
            cube([122, 8, 6-d]);
            translate([0, 6, 6])
            cube([122, 2, 6]);
        }
        pcb(false);
    }
    if (demo) {
        pcb(false);
    }
}

box_lower(true, false);
cap();
bracket(false);

*difference() {
    box_lower(true, false);
    translate([-d, cav_y/2 + wall, -d])
    cube([cav_x + wall*4 + d*2, cav_y, cav_z_lower + wall +d*2]);
}
