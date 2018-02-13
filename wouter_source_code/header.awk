BEGIN        { c = 0 }
/^int/       { c = 1 }   # starting point
/^double/    { c = 1 }
/main/       { exit }    # exclude
/\{/         { exit }    # end point
//           { if ( c==1 ) { print $0 } }
