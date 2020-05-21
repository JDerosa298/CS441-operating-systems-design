# input to "bubble-generator"s

set all_graphs {

  mash-program {
    loop
      {line
        {or
          mode-line
          constant-line
          variable-line
          address-line
          label-line
          code-line
          raw-line
          {}
        }
      }
      /NEWLINE
  }

  mode-line {
    line
      \".MODE\" \"NOP_DELAY\" {or \"OFF\" \"ON\"}
  }

  constant-line {
    line {or \".CONST\" \".CONSTANT\"} identifier constant-immediate
  }

  variable-line {
    line
      {or \".VAR\" \".VARIABLE\"}
      identifier
      {or
        {}
        {line
          \"[\"
          number
          {or {} {line \",\" number}}
          \"]\"
        }
      }
  }

  address-line {
    line {or \".ADDR\" \".ADDRESS\"} address-immediate
  }

  label-line {
    line label
  }

  raw-line {
    loop {or constant-immediate string packed-string} {}
  }

  code-line {
    line
      {or {} predicate}
      {or
        alu-code-line
        data-code-line
        control-code-line
        int-code-line
        int-return-code-line
        pseudoinstruction-code-line
        complete-code-line
      }
  }

  predicate {
    line \"_\ \"
  }

  pseudoinstruction-code-line {
    or
      neg-code-line
      inv-code-line
      inc-code-line
      dec-code-line
      movr-code-line
      movi-code-line
      bra-code-line
      nop-code-line
      flush-code-line
  }

  alu-code-line {
    line
      {or
        \"ADD\"
        \"SUB\"
        \"MULT\"
        \"DIV\"
        \"MOD\"
        \"AND\"
        \"OR\"
        \"EXOR\"
        \"SHL\"
        \"SHR\"
      }
      register \"=\" register
      {or
        \"+\"
        \"-\"
        \"*\"
        \"/\"
        \"%\"
        \"&\"
        \"|\"
        \"^\"
        \"<<\"
        \">>\"
      }
      paren-reg-immed-expr
  }

  paren-reg-immed-expr {
    or
      {line \"(\" reg-immediate-expr \")\"}
      register
      immediate
  }

  optional-paren-reg-immed-expr {
    or
      {line \"(\" reg-immediate-expr \")\"}
      reg-immediate-expr
  }

  reg-immediate-expr {
    or
      {line
        register
        {or
          {}
          {line {or \"+\" \"-\"} immediate}
        }
      }
      immediate
  }

  data-code-line {
    or
      {line
        {or \"LOAD\" \"RLOAD\"}
        register \"=\" bracket-reg-immed-expr
      }
      {line
        {or \"STOR\" \"RSTOR\"}
        bracket-reg-immed-expr \"=\" register
      }
  }

  control-code-line {
    line
      {or
        \"BRAE\"
        \"BRANE\"
        \"BRAL\"
        \"BRALE\"
        \"BRAG\"
        \"BRAGE\"
      }
      \"[\" reg-immediate-expr {or {} \"T\" \"F\"} \"]\"
      {or
        \"==\"
        \"!=\"
        \"<\"
        \"<=\"
        \">\"
        \">=\"
      }
      register
  }

  int-code-line {
    line \"INT\" optional-paren-reg-immed-expr
  }

  int-return-code-line {
    line \"IRET\"
  }

  neg-code-line {
    line \"NEG\" register
  }

  inv-code-line {
    line \"INV\" register
  }

  inc-code-line {
    line \"INC\" register
  }

  dec-code-line {
    line \"DEC\" register
  }

  movr-code-line {
    line \"MOVR\" register EQUALS register
  }

  movi-code-line {
    line \"MOVI\" register EQUALS immediate
  }

  bra-code-line {
    line \"BRA\" \"[\" reg-immediate-expr {or {} \"T\" \"F\"} \"]\"
  }

  nop-code-line {
    line \"NOP\"
  }

  flush-code-line {
    line \"FLUSH\"
  }

  complete-code-line {
    line
      {or
        \"ADD\"
        \"SUB\"
        \"MULT\"
        \"DIV\"
        \"MOD\"
        \"AND\"
        \"OR\"
        \"EXOR\"
        \"SHL\"
        \"SHR\"
        \"LOAD\"
        \"RLOAD\"
        \"STOR\"
        \"RSOTR\"
        \"BRAE\"
        \"BRANE\"
        \"BRAL\"
        \"BRALE\"
        \"BRAG\"
        \"BRAGE\"
      }
      register
      {or {} \",\"}
      register
      {or {} \",\"}
      register
      {or {} \",\"}
      immediate
  }

  immediate {
    or
      number
      character
      label
      identifier
  }

  constant-immediate {
    or
      number
      character
  }

  address-immediate {
    line number
  }

  number {
    line
      {or {} \"+\" \"-\"}
      {or decimal-value binary-value octal-value hexadecimal-value}
  }

  decimal-value {
    or
      {line
        {loop
          DECIMAL-DIGIT {}
        }
        {loop
          {}
          {
            {loop \"_\" {}}
            {loop DECIMAL-DIGIT {}}
          }
        }
      }
      {line
        \"0d\"
        {loop
          {line
            {loop {} \"_\"}
            DECIMAL-DIGIT
          }
          {}
        }
      }
  }

  binary-value {
    line
      \"0b\"
      {loop
        {line
          {loop {} \"_\"}
          BINARY-DIGIT
        }
        {}
      }
  }

  octal-value {
    line
      \"0c\"
      {loop
        {line
          {loop {} \"_\"}
          OCTAL-DIGIT
        }
        {}
      }
  }

  hexadecimal-value {
    line
      \"0x\"
      {loop
        {line
          {loop {} \"_\"}
          HEXADECIMAL-DIGIT
        }
        {}
      }
  }

  label {
    line \"!\" identifier
  }

  identifier {
    line
      LETTER
      {loop {or {} LETTER DECIMAL-DIGIT \"_\"} {}}
  }

  character {
    line \' PRINTABLE-CHARACTER \'
  }

  string {
    line \'\' {loop {} PRINTABLE-CHARACTER} \'\'
  }

  packed-string {
    line \' {loop {} PRINTABLE-CHARACTER} \'
  }

  comment {
    or
      {line \"#\" {loop {} ANYTHING-EXCEPT-NEWLINE}
        {or NEWLINE END-OF-INPUT}}
      {line \"<<<\" {loop {} ANYTHING-EXCEPT-\">>>\"} \">>>\"}
  }

}