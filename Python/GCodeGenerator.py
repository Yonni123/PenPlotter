import Python.RasterToVector as RTV


def header():
    return ""


# Recursively generate GCode for a single polygon
def poly_to_gcode(poly, x_pos, y_pos, pen_up, pen_down):
    gcode = ""
    points = poly.points
    # Move to the first point
    gcode += "G0 X" + str(points[0, 0] + x_pos) + " Y" + str(points[0, 1] + y_pos) + "\n"
    # Lower the pen
    gcode += pen_down + "\n"
    # Move to the rest of the points
    for i in range(1, len(points)):
        gcode += "G1 X" + str(points[i, 0] + x_pos) + " Y" + str(points[i, 1] + y_pos) + "\n"
    # Move back to the first point
    gcode += "G1 X" + str(points[0, 0] + x_pos) + " Y" + str(points[0, 1] + y_pos) + "\n"
    # Lift the pen
    gcode += pen_up + "\n"

    # Recursively generate GCode for the children
    for child in poly.children:
        gcode += poly_to_gcode(child, x_pos, y_pos, pen_up, pen_down)
    return gcode


def get_gcode(polygons, x_pos, y_pos, pen_up, pen_down):
    gcode = header()
    for poly in polygons.children:
        gcode += poly_to_gcode(poly, x_pos, y_pos, pen_up, pen_down)
    return gcode
