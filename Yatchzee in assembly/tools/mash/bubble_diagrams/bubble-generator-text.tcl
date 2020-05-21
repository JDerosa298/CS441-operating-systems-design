#!/usr/bin/env tclsh
#
# Run this TCL script to generate syntax bubble diagrams from
# text descriptions.
#
# This version generates a pure HTML table based version with
# no graphics, all in one file named "all-text.html".  It may
# be useful to people wanting text search or screen reader 
# support.

source [file join [file dirname [info script]] bubble-generator-text-data.tcl]

# used for curves
set rad "4pt"
set rad1 "5pt"
# used for bubble text (<= (2 x rad)+2)
set font1 "9pt"
# used for arrows (~ 3 x rad)
set font2 "12pt"
# used for everything else (~ font1+5)
set font3 "14pt"

# |
set c9474 "<table><tr><td class=\"rside\"></td><td class=\"empty\"></td></tr><tr><td class=\"rside\"></td><td class=\"empty\"></td></tr></table>"
# -|
set c9488 "<table><tr><td class=\"bottom\"></td><td class=\"empty\"></td></tr><tr><td class=\"empty\"></td><td class=\"lside\"></td></tr></table>"
# |-
set c9484 "<table><tr><td class=\"empty\"></td><td class=\"empty\"></td></tr><tr><td class=\"rside\" style=\"padding-bottom:$rad1;\"></td><td class=\"top\"></td></tr></table>"
# |_
set c9492 "<table><tr><td class=\"rside\"></td><td class=\"bottom\"></td></tr><tr><td class=\"empty\"></td><td class=\"empty\"></td></tr></table>"
# _|
set c9496 "<table><tr><td class=\"bottom\"></td><td class=\"lside\"></td></tr><tr><td class=\"empty\"></td><td class=\"empty\"></td></tr></table>"
#-(- top left
set c9581 "<table><tr><td class=\"bottom\"></td><td class=\"bottom\"></td></tr><tr><td class=\"empty\"></td><td class=\"empty\" style=\"border-top-style:solid; border-left-style:solid; border-width:1px; border-top-left-radius:$rad; -moz-border-radius-topleft:$rad; -webkit-border-top-left-radius:$rad;\"></td></tr></table>"
#-)- top right
set c9582 "<table><tr><td class=\"bottom\"></td><td class=\"bottom\"></td></tr><tr><td class=\"empty\" style=\"border-top-style:solid; border-right-style:solid; border-width:1px; border-top-right-radius:$rad; -moz-border-radius-topright:$rad; -webkit-border-top-right-radius:$rad;\"></td><td class=\"empty\"></td></tr></table>"
#-|-
set c9516 "<table><tr><td class=\"bottom\"></td><td class=\"empty\"></td></tr><tr><td class=\"rside\"></td><td class=\"top\"></td></tr></table>"
#( bot left
set c9584 "<table><tr><td class=\"empty\"></td><td class=\"empty\" style=\"border-bottom-style:solid; border-left-style:solid; border-width:1px; border-bottom-left-radius:$rad; -moz-border-radius-bottomleft:$rad; -webkit-border-bottom-left-radius:$rad;\"></td></tr><tr><td class=\"empty\"></td><td class=\"empty\"></td></tr></table>"
# ) bot right
set c9583 "<table><tr><td class=\"empty\" style=\"border-bottom-style:solid; border-right-style:solid; border-width:1px; border-bottom-right-radius:$rad; -moz-border-radius-bottomright:$rad; -webkit-border-bottom-right-radius:$rad;\"></td><td class=\"empty\"></td></tr><tr><td class=\"empty\"></td><td class=\"empty\"></td></tr></table>"
# |-
set c9500 "<table><tr><td class=\"rside\"></td><td class=\"bottom\"></td></tr><tr><td class=\"rside\"></td><td class=\"empty\"></td></tr></table>"
# -|
set c9508 "<table><tr><td class=\"bottom\"></td><td class=\"lside\"></td></tr><tr><td class=\"empty\"></td><td class=\"lside\"></td></tr></table>"
#->
set rarr  "<table><tr><td class=\"bottom\"></td><td rowspan=2 style=\"vertical-align:middle; font-size:$font2;\">&gt;</td></tr><tr><td class=\"empty\"></td></tr></table>"
# <-
set larr  "<table><tr><td rowspan=2 style=\"vertical-align:middle; font-size:$font2;\">&lt;</td><td class=\"bottom\"></td></tr><tr><td class=\"empty\"></td></tr></table>"
# ---
set hr  "<table width=100%><tr><td width=100% class=\"bottom\"></td></tr><tr><td width=100% class=\"empty\"></td></tr></table>"


# Draw a bubble containing $txt. 
#
proc draw_bubble {txt} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  if {$txt=="nil"} {
    return [list 1 "<table width=100%><tr><td width=100%>$hr</td></tr></table>"]
  } elseif {$txt=="bullet"} {
    return [list 1 "<table><tr><td>&otimes;</td></tr></table>"]
  }

  if {[regexp {^/[a-z]} $txt]} {
    set txt [string range $txt 1 end]
    set istoken 1
  } elseif {[regexp {^[a-z]} $txt]} {
    set istoken 0
  } else {
    set istoken 1
  }

  set href "&nbsp;<a href=\"#$txt\">$txt</a>&nbsp;"
  set txt "&nbsp;$txt&nbsp;"

  if {$istoken} {
    return [list 1 "<table><tr><td class=\"round\">$txt</td></tr></table>"]
  } else {
    return [list 1 "<table><tr><td class=\"square\">$href</td></tr></table>"]
  }

  return [list 0 ""]
}

# Draw a sequence of terms from left to write.  Each element of $lx
# descripts a single term.
#
proc draw_line {lx} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set n [llength $lx]

  set h 1
  set content ""
  set i 0
  foreach term $lx {
    incr i

    set rc [draw_diagram $term]
    set th [lindex $rc 0]
    set tcontent [lindex $rc 1]
    if { $th > $h } { set h $th }

    if {$i != $n} {
      set content "$content<td>$tcontent</td><td>$rarr</td>"
    } else {
      set content "$content<td>$tcontent</td>"
    }
  }

  set content "<table><tr>$content</tr></table>"

  return [list $h $content]
}

# Draw a sequence of terms from right to left.
#
proc draw_backwards_line {lx} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set n [llength $lx]

  set h 1
  if {$n < 1} {
    set content "<td width=100%>$hr</td>"
  } {
    set content ""
    set i 0
    foreach term $lx {
      incr i

      set rc [draw_diagram $term 1]
      set th [lindex $rc 0]
      set tcontent [lindex $rc 1]
      if { $th > $h } { set h $th }

      if {$i != $n} {
        set content "<td>$larr</td><td>$tcontent</td>$content"
      } else {
        set content "<td>$tcontent</td>$content"
      }
    }
  }
  set content "<table width=100%><tr>$content<td width=100%>$hr</td></tr></table>"

  return [list $h $content]
}

# Draw a sequence of terms from top to bottom.
#
proc draw_stack {indent lx} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set n [llength $lx]

  set h 0
  set content "<table>"

  set i 0
  foreach term $lx {
    set rc [draw_diagram $term 1]
    set th [lindex $rc 0]
    set tcontent [lindex $rc 1]
    set h [ expr { $h + $th } ]

    set v [ expr { $th - 1 } ]
    set vert [string repeat "<tr><td>$c9474</td><tr>" $v]

    incr i
    if {$i == 1} {
      set down "<table><tr><td>$c9488</td></tr>$vert</table>"
      set up "<table><tr><td>$c9484</td></tr>$vert</table>"
      set content "$content<tr><td>$hr</td><td>$hr</td><td>$tcontent</td><td>$rarr</td><td>$down</td><td>$up</td></tr>"
    } elseif {$i == $n} {
      set content "$content<tr><td>$c9492</td><td>$rarr</td><td>$tcontent</td><td>$hr</td><td>$rarr</td><td>$c9496</td></tr>"
    } else { 
      set down "<table><tr><td>$c9488</td></tr>$vert</table>"
      set up "<table><tr><td>$c9474</td></tr>$vert</table>"
      set content "$content<tr><td>$c9492</td><td>$rarr</td><td>$tcontent</td><td>$rarr</td><td>$down</td><td>$up</td></tr>"
    }
    if {$i != $n} {
      incr h
      set content "$content<tr><td>$c9484</td><td>$larr</td><td width=100%>$hr</td><td>$larr</td><td>$c9496</td><td>$c9474</td></tr>"
    }
  }
  set content "$content</table>"

  return [list $h $content]
}

proc draw_loop {forward back} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set h 0
  set content "<table>"

  set rc [draw_diagram $forward 1]
  set th [lindex $rc 0]
  set tcontent [lindex $rc 1]

  set v [ expr { $th - 1 } ]
  set vert [string repeat "<tr><td>$c9474</td><tr>" $v]

  set up "$c9581"
  set down "$c9582"
  if { $v > 0 } {
    set up "<table><tr><td>$up</td></tr>$vert</table>"
    set down "<table><tr><td>$down</td></tr>$vert</table>"
  }
  
  set h [ expr { $h + $th } ]
  set content "$content<tr><td>$up</td><td>$rarr</td><td>$tcontent</td><td>$rarr</td><td>$down</td></tr>"

  set rc [draw_backwards_line $back]
  set th [lindex $rc 0]
  set tcontent [lindex $rc 1]

  set h [ expr { $h + $th } ]
  set content "$content<tr><td>$c9584</td><td>$larr</td><td>$tcontent</td><td>$larr</td><td>$c9583</td></tr>"

  set content "$content</table>"

  return [list $h $content]
}

proc draw_toploop {forward back} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set h 0
  set content "<table>"

  set rc [draw_diagram $forward 1]
  set th [lindex $rc 0]
  set tcontent [lindex $rc 1]

  set v [ expr { $th - 1 } ]
  set vert [string repeat "<tr><td>$c9474</td><tr>" $v]

  set up "$c9581"
  set down "$c9582"
  if { $v > 0 } {
    set up "<table><tr><td>$up</td></tr>$vert</table>"
    set down "<table><tr><td>$down</td></tr>$vert</table>"
  }
  
  set h [ expr { $h + $th } ]
  set content "$content<tr><td>$up</td><td>$rarr</td><td>$tcontent</td><td>$rarr</td><td>$down</td></tr>"

  set rc [draw_backwards_line $back]
  set th [lindex $rc 0]
  set tcontent [lindex $rc 1]

  set h [ expr { $h + $th } ]
  set content "$content<tr><td>$c9584</td><td>$larr</td><td>$tcontent</td><td>$larr</td><td>$c9583</td></tr>"

  set content "$content</table>"

  return [list $h $content]
}

proc draw_or {lx} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set n [llength $lx]

  set h 0
  if {$n < 1} {
    set content "<tr><td width=100%>$hr</td></tr>"
  } else {
    set content ""
    set i 0
    foreach term $lx {

      set rc [draw_diagram $term 1]
      set th [lindex $rc 0]
      set tcontent [lindex $rc 1]

      set v [ expr { $th - 1 } ]
      set vert [string repeat "<tr><td>$c9474</td><tr>" $v]

      incr i
      if {$i == 1} {
        set beg "<tr><td>$c9582</td><tr>"
        set end "<tr><td>$c9581</td><tr>"
      } elseif {$i == $n} {
        set beg "<tr><td>$c9584</td><tr>"
        set end "<tr><td>$c9583</td><tr>"
        set vert ""
      } else { 
        set beg "<tr><td>$c9500</td><tr>"
        set end "<tr><td>$c9508</td><tr>"
      }
      
      set h [ expr { $h + $th } ]
      set content "$content<tr><td><table>$beg$vert</table></td><td>$rarr</td><td>$tcontent</td><td>$rarr</td><td><table>$end$vert</table></td></tr>"
    }
  }
  set content "<table>$content</table>"
  return [list $h $content]
}

proc draw_diagram {spec {draw_hr 0}} {
  global c9474 c9488 c9484 c9492 c9496 c9581 c9582 c9516 c9584 c9583 c9500 c9508 rarr larr hr

  set n [llength $spec]
  set cmd [lindex $spec 0]

  if {$n==1} {
    set rc [draw_bubble $spec]
  } elseif {$n==0} {
    set rc [draw_bubble nil]
  } elseif {$cmd=="line"} {
    set rc [draw_line [lrange $spec 1 end]]
  } elseif {$cmd=="stack"} {
    set rc [draw_stack 0 [lrange $spec 1 end]]
  } elseif {$cmd=="indentstack"} {
    set rc [draw_stack $::HSEP [lrange $spec 1 end]]
  } elseif {$cmd=="loop"} {
    set rc [draw_loop [lindex $spec 1] [lindex $spec 2]]
  } elseif {$cmd=="toploop"} {
    set rc [draw_toploop [lindex $spec 1] [lindex $spec 2]]
  } elseif {$cmd=="or"} {
    set rc [draw_or [lrange $spec 1 end]]
  } elseif {$cmd=="opt"} {
    set args [lrange $spec 1 end]
    if {[llength $args]==1} {
      set rc [draw_or [list nil [lindex $args 0]]]
    } else {
      set rc [draw_or [list nil "line $args"]]
    }
  } elseif {$cmd=="optx"} {
    set args [lrange $spec 1 end]
    if {[llength $args]==1} {
      set rc [draw_or [list [lindex $args 0] nil]]
    } else {
      set rc [draw_or [list "line $args" nil]]
    }
  } elseif {$cmd=="tailbranch"} {
    # set rc [draw_tail_branch [lrange $spec 1 end]]
    set rc [draw_or [lrange $spec 1 end]]
  } else {
    error "unknown operator: $cmd"
  }

  set h [lindex $rc 0]
  set content [lindex $rc 1]

  if {$draw_hr} {
    set content "<table width=100%><tr><td>$content</td><td width=100%>$hr</td></tr></table>"
  } {
    set content "<table><tr><td>$content</td></tr></table>"
  }

  return [list $h $content]
}

proc draw_graph {name spec} {
  set rc [ draw_diagram "line bullet [list $spec] bullet"]
  set h [lindex $rc 0]
  set content [lindex $rc 1]
  return [list $h $content]
}

set f [open all-text.html w]
#puts $f "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"
#puts $f "<html>"
#puts $f "<head>"
puts $f "<style type='text/css'>"
#puts $f "h2 { font-family:helvetica; font-size:$font3; }"
puts $f "table, td { empty-cells:show; border-collapse:separate; border-style:none; margin:0px; padding:0px; border-spacing:0px; }"
puts $f "td { vertical-align:top; white-space:nowrap; font-family:helvetica; font-size:$font3; }"
puts $f ".round { font-size:$font1; border-style:solid; border-width:1px; padding:1px; border-radius:$font1; -moz-border-radius:$font1; -webkit-border-radius:$font1; }"
puts $f ".square { font-size:$font1; border-style:solid; border-width:1px; padding:1px; }"
puts $f ".top { padding:$rad; border-top-style:solid; border-width:1px; }"
puts $f ".bottom { padding:$rad; border-bottom-style:solid; border-width:1px; }"
puts $f ".rside { padding:$rad; border-right-style:solid; border-width:1px; padding-top:$rad1; }"
puts $f ".lside { padding:$rad; border-left-style:solid; border-width:1px; padding-top:$rad1; }"
puts $f ".empty { padding:$rad; }"
puts $f "</style>"
#puts $f "</head>"
#puts $f "<body>"
puts $f ""
foreach {name graph} $all_graphs {
  if {$name == "sql-stmt-list" || 1} {
    puts $f "<a name=\"$name\"> </a>"
    puts $f "<h2>$name:</h2>"
    set rc [draw_graph $name $graph]
    set h [lindex $rc 0]
    set content [lindex $rc 1]
    puts $f "$content"
  }
}
#puts $f "</body>"
#puts $f "</html>"
close $f

