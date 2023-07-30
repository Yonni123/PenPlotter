import Python.RasterToVector as RTV


def header(pen_up):
    return """M107 ;Turn off fan
G28 X Y Z; home all axes
G0 Z""" + str(pen_up) + """ F6000 ;Move Z to pen up position
"""


def footer(pen_up):
    return """G0 Z""" + str(pen_up) + """ F6000 ;Move Z to pen up position
G28 X;"""


# Recursively generate GCode for a single polygon
def poly_to_gcode(poly, x_pos, y_pos, pen_up, pen_down):
    gcode = ""
    points = poly.points
    # Move to the first point
    gcode += "G0 X" + str(points[0, 0] + x_pos) + " Y" + str(points[0, 1] + y_pos) + " F1800\n"
    # Lower the pen
    gcode += "G0 Z" + str(pen_down) + " F6000\n"
    # Move to the rest of the points
    for i in range(1, len(points)):
        gcode += "G1 X" + str(points[i, 0] + x_pos) + " Y" + str(points[i, 1] + y_pos) + " F1800\n"
    # Move back to the first point
    gcode += "G1 X" + str(points[0, 0] + x_pos) + " Y" + str(points[0, 1] + y_pos) + " F1800\n"
    # Lift the pen
    gcode += "G0 Z" + str(pen_up) + " F6000\n"

    # Recursively generate GCode for the children
    for child in poly.children:
        gcode += poly_to_gcode(child, x_pos, y_pos, pen_up, pen_down)

    return gcode


def get_gcode(polygons, x_pos, y_pos, pen_up, pen_down):
    gcode = header(pen_up)
    for poly in polygons.children:
        gcode += poly_to_gcode(poly, x_pos, y_pos, pen_up, pen_down)
    gcode += footer(pen_up)
    return gcode
