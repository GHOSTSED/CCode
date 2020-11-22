var CWPM_TokenType = {END: 0,NUMBER: 1,STRING: 2,OPENBRACE: 3,CLOSEBRACE: 4,OPENBRACKET: 5,CLOSEBRACKET: 6,OPENPAREN: 7,CLOSEPAREN: 8,SEMICOLON: 9,COMMA: 10,DOT: 11,COLON: 12,QUESTION: 13,ADD: 14,SUBTRACT: 15,MUTIPLY: 16,DIVIDE: 17,DADD: 18,DSUBTRACT: 19,ADD_ASSIGNMENT: 20,ASSIGNMENT: 21,MODULO: 22,EQUAL: 23,CONGRUENT: 24,NOT_CONGRUENT: 25,NOT_EQUAL: 26,NOT: 27,BIT_AND: 28,BIT_OR: 29,LOGICAL_AND: 30,LOGICAL_OR: 31,BIT_LEFT: 32,BIT_RIGHT: 33,BIT_NOT: 34,BIT_AND_OR: 35,REGULAR: 36,GREATER: 37,LESS: 38,NOT_IDENTIFIED: 39,KEYWORD_DO: 40,KEYWORD_IF: 41,KEYWORD_IN: 42,KEYWORD_FOR: 43,KEYWORD_NEW: 44,KEYWORD_TRY: 45,KEYWORD_VAR: 46,KEYWORD_CASE: 47,KEYWORD_ELSE: 48,KEYWORD_WITH: 49,KEYWORD_BREAK: 50,KEYWORD_CATCH: 51,KEYWORD_CONST: 52,KEYWORD_THROW: 53,KEYWORD_WHILE: 54,KEYWORD_DELETE: 55,KEYWORD_EXPORT: 56,KEYWORD_RETURN: 57,KEYWORD_SWITCH: 58,KEYWORD_DEFAULT: 59,KEYWORD_FINALLY: 60,KEYWORD_CONTINUE: 61,KEYWORD_FUNCTION: 62,KEYWORD_INSTANCEOF: 63,KEYWORD_OBJECT: 64,KEYWORD_INTERNAL_FUNCTION: 65,KEYWORDS_NO_FOUND: 90,TYPE_KEYWORDS_BASE: 100};
CWPM_JsParserData.prototype.SCANNING = 1;
CWPM_JsParserData.prototype.CASCADING = 2;
CWPM_JsParserData.prototype.CWPM_RewriteEval = 1000;
CWPM_JsParserData.prototype.CWPM_Rewrite = 1001;
CWPM_JsParserData.prototype.CWPM_RewriteWith = 1002;
CWPM_JsParserData.prototype.CWPM_RewriteNo = 1003;
function CWPM_JsToken() {
    this.jsBeginPosition = 0;
    this.tokenBeginPosition = 0;
    this.tokenLength = 0;
    this.tokenType = 0
}
function CWPM_JsParserData(a) {
    this.jsString = a;
    this.jsLength = a.length;
    this.currentPosition = 0;
    this.token = new CWPM_JsToken();
    this.savedToken = 0;
    this.parseFlag = 0;
    this.rewriteFlag = 0;
    this.appendFlag = 0;
    this.parseWithFlag = 0;
    this.rewriteWithFlag = 0;
    this.originalString = a;
    this.rewritenToken = new Array();
    this.currentRewritenToken = 0;
    this.currentMethodIndex = 0;
    this.methods = new Array();
    this.currentPropIndex = 0;
    this.props = new Array();
    this.indexInWithState = 0;
    this.quote = '"';
    this.state = 0;
    this.returnFlag = 0;
    this.returnPosition = 0;
    this.withEndPosition = 0
}
CWPM_JsParserData.prototype.setFlag = function(a) {
    this.state |= a
};
CWPM_JsParserData.prototype.unsetFlag = function(a) {
    this.state &= ~a
};
CWPM_JsParserData.prototype.CWPM_SkipSpace = function() {
    while (this.currentPosition < this.jsLength) {
        var a = this.jsString.charAt(this.currentPosition);
        if (a == " " || a == "\t" || a == "\r" || a == "\n") {
            this.currentPosition++
        } else {
            break
        }
    }
};
CWPM_JsParserData.prototype.CWPM_GetChar = function() {
    if (this.currentPosition < this.jsLength) {
        var a = this.jsString.charAt(this.currentPosition++);
        this.token.tokenLength++;
        return a
    }
    return "\0"
};
CWPM_JsParserData.prototype.CWPM_GetCurrentChar = function() {
    if (this.currentPosition < this.jsLength) {
        var a = this.jsString.charAt(this.currentPosition);
        return a
    }
    return "\0"
};
CWPM_JsParserData.prototype.CWPM_IsKeyword = function(b) {
    if (!b) {
        return -1
    }
    var a = b.length;
    switch (a) {
        case 2:
            if ("do" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_DO
            } else {
                if ("if" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_IF
                } else {
                    if ("in" == b) {
                        return this.token.tokenType = CWPM_TokenType.KEYWORD_IN
                    }
                }
            }
            break;
        case 3:
            if ("for" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_FOR
            } else {
                if ("new" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_NEW
                } else {
                    if ("try" == b) {
                        return this.token.tokenType = CWPM_TokenType.KEYWORD_TRY
                    } else {
                        if ("var" == b) {
                            return this.token.tokenType = CWPM_TokenType.KEYWORD_VAR
                        }
                    }
                }
            }
            break;
        case 4:
            if ("case" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_CASE
            } else {
                if ("else" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_ELSE
                } else {
                    if ("with" == b) {
                        return this.token.tokenType = CWPM_TokenType.KEYWORD_WITH
                    } else {
                        if ("NULL" == b) {
                            return this.token.tokenType = CWPM_TokenType.KEYWORD_OBJECT
                        } else {
                            if ("this" == b) {
                                return this.token.tokenType = CWPM_TokenType.KEYWORD_OBJECT
                            } else {
                                if ("true" == b) {
                                    return this.token.tokenType = CWPM_TokenType.KEYWORD_OBJECT
                                } else {
                                    if ("void" == b) {
                                        return this.token.tokenType = CWPM_TokenType.KEYWORD_INTERNAL_FUNCTION
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 5:
            if ("break" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_BREAK
            } else {
                if ("catch" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_CATCH
                } else {
                    if ("const" == b) {
                        return this.token.tokenType = CWPM_TokenType.KEYWORD_CONST
                    } else {
                        if ("throw" == b) {
                            return this.token.tokenType = CWPM_TokenType.KEYWORD_THROW
                        } else {
                            if ("while" == b) {
                                return this.token.tokenType = CWPM_TokenType.KEYWORD_WHILE
                            } else {
                                if ("false" == b) {
                                    return this.token.tokenType = CWPM_TokenType.KEYWORD_OBJECT
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 6:
            if ("delete" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_DELETE
            } else {
                if ("export" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_EXPORT
                } else {
                    if ("return" == b) {
                        return this.token.tokenType = CWPM_TokenType.KEYWORD_RETURN
                    } else {
                        if ("switch" == b) {
                            return this.token.tokenType = CWPM_TokenType.KEYWORD_SWITCH
                        } else {
                            if ("typeof" == b) {
                                return this.token.tokenType = CWPM_TokenType.KEYWORD_INTERNAL_FUNCTION
                            }
                        }
                    }
                }
            }
            break;
        case 7:
            if ("default" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_DEFAULT
            } else {
                if ("finally" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_FINALLY
                }
            }
            break;
        case 8:
            if ("continue" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_CONTINUE
            } else {
                if ("function" == b) {
                    return this.token.tokenType = CWPM_TokenType.KEYWORD_FUNCTION
                }
            }
            break;
        case 10:
            if ("instanceof" == b) {
                return this.token.tokenType = CWPM_TokenType.KEYWORD_INSTANCEOF
            }
            break
    }
    return -1
};
CWPM_JsParserData.prototype.CWPM_CheckChar = function(b) {
    if (this.currentPosition >= this.jsLength) {
        return false
    }
    var a = this.jsString.charAt(this.currentPosition);
    if (a == b) {
        this.currentPosition++;
        this.token.tokenLength++;
        return true
    }
    return false
};
CWPM_JsParserData.prototype.CWPM_GetOneToken = function() {
    if (this.savedToken) {
        return this.token = this.savedToken, this.savedToken = 0, this.token.tokenType
    }
    this.token.jsBeginPosition = this.currentPosition;
    var e = 0;
    var h = 0;
    var d = 1;
    while (d) {
        this.CWPM_SkipSpace();
        if (this.currentPosition >= this.jsLength) {
            return this.token.tokenType = CWPM_TokenType.END
        }
        this.token.tokenLength = 0;
        this.token.tokenBeginPosition = this.currentPosition;
        h = this.CWPM_GetChar();
        if (h == "\0") {
            return this.token.tokenType = CWPM_TokenType.END
        }
        if ((h >= "a" && h <= "z") || (h >= "A" && h <= "Z") || h == "_" || h == "$") {
            do {
                h = this.CWPM_GetChar()
            } while ((h >= "a" && h <= "z") || (h >= "A" && h <= "Z") || h == "_" || h == "$" || (h >= "0" && h <= "9"));
            if (h != "\0") {
                this.currentPosition--, this.token.tokenLength--
            }
            e = this.jsString.substr(this.token.tokenBeginPosition, this.token.tokenLength);
            var b = this.CWPM_IsKeyword(e);
            if (-1 == b) {
                this.token.tokenType = CWPM_TokenType.KEYWORDS_NO_FOUND
            }
        } else {
            var a = this.CWPM_GetCurrentChar();
            if (h >= "0" && h <= "9" || (h == "." && (a >= "0" && a <= "9"))) {
                if (h == "0") {
                    if (a == "x" || a == "X") {
                        do {
                            h = this.CWPM_GetChar()
                        } while (h >= "0" && h <= "9" || h >= "a" && h <= "f" || h >= "A" && h <= "F");
                        if (h != "\0") {
                            this.currentPosition--;
                            this.token.tokenLength--
                        }
                        return this.token.tokenType = CWPM_TokenType.NUMBER
                    }
                }
                do {
                    h = this.CWPM_GetChar()
                } while (h >= "0" && h <= "9");
                if (h == ".") {
                    do {
                        h = this.CWPM_GetChar()
                    } while (h >= "0" && h <= "9")
                }
                if (h == "e" || h == "E") {
                    h = this.CWPM_GetChar();
                    if (h == "+" || h == "-") {
                        h = this.CWPM_GetChar()
                    }
                    do {
                        h = this.CWPM_GetChar()
                    } while (h >= "0" && h <= "9")
                }
                if (h != "\0") {
                    this.currentPosition--;
                    this.token.tokenLength--
                }
                this.token.tokenType = CWPM_TokenType.NUMBER
            } else {
                if (h == "'" || h == '"') {
                    var g = h;
                    this.token.tokenLength = 1;
                    h = this.CWPM_GetChar();
                    while (h != g && h != "\0") {
                        if (h == "\\") {
                            h = this.CWPM_GetChar()
                        }
                        h = this.CWPM_GetChar()
                    }
                    this.token.tokenType = CWPM_TokenType.STRING
                } else {
                    switch (h) {
                        case "+":
                            if (this.CWPM_CheckChar("+")) {
                                return this.token.tokenType = CWPM_TokenType.DADD
                            }
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.ADD_ASSIGNMENT
                            }
                            return this.token.tokenType = CWPM_TokenType.ADD;
                        case "-":
                            if (this.CWPM_CheckChar("-")) {
                                if (this.CWPM_CheckChar(">")) {
                                    while (this.currentPosition < this.jsLength) {
                                        var f = this.jsString.charAt(this.currentPosition++);
                                        if (f == "\n") {
                                            break
                                        }
                                    }
                                    continue
                                }
                                return this.token.tokenType = CWPM_TokenType.DSUBTRACT
                            }
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.SUBTRACT_ASSIGNMENT
                            }
                            return this.token.tokenType = CWPM_TokenType.SUBTRACT;
                        case "*":
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.MUTIPLY_ASSIGNMENT
                            }
                            return this.token.tokenType = CWPM_TokenType.MUTIPLY;
                        case "/":
                            if (this.CWPM_CheckChar("/")) {
                                while (this.currentPosition < this.jsLength) {
                                    var f = this.jsString.charAt(this.currentPosition++);
                                    if (f == "\n") {
                                        break
                                    }
                                }
                                continue
                            }
                            if (this.CWPM_CheckChar("*")) {
                                do {
                                    h = this.CWPM_GetChar();
                                    if (h == "\0") {
                                        return this.token.tokenType = CWPM_TokenType.END
                                    }
                                } while (h != "*" || !this.CWPM_CheckChar("/"))
                            }
                            if (this.state & this.SCANNING) {
                                do {
                                    h = this.CWPM_GetChar();
                                    if (h == "\0" || h == "\r" || h == "\n") {
                                        return this.token.tokenType = CWPM_TokenType.REGULAR
                                    }
                                    if (h == "\\") {
                                        this.CWPM_GetChar()
                                    }
                                } while (h != "/");
                                while (this.CWPM_CheckChar("g") || this.CWPM_CheckChar("i") || this.CWPM_CheckChar("m")) {
                                }
                                return this.token.tokenType = CWPM_TokenType.REGULAR
                            }
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.DIVIDE_ASSIGNMENT
                            }
                            return this.token.token_type = this.DIVIDE;
                        case "%":
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.MODULO_ASSIGNMENT
                            }
                            return this.token.tokenType = CWPM_TokenType.MODULO;
                        case "^":
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.BIT_NOT_ASSIGNMENT
                            }
                            return this.token.tokenType = CWPM_TokenType.BIT_AND_OR;
                        case "&":
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.BIT_AND_ASSIGNMENT
                            }
                            if (this.CWPM_CheckChar("&")) {
                                return this.token.tokenType = CWPM_TokenType.LOGICAL_AND
                            }
                            return this.token.tokenType = CWPM_TokenType.BIT_AND;
                        case "|":
                            if (this.CWPM_CheckChar("=")) {
                                return this.token.tokenType = CWPM_TokenType.BIT_OR_ASSIGNMENT
                            }
                            if (this.CWPM_CheckChar("|")) {
                                return this.token.tokenType = CWPM_TokenType.LOGICAL_OR
                            }
                            return this.token.tokenType = CWPM_TokenType.BIT_OR;
                        case "!":
                            if (this.CWPM_CheckChar("=")) {
                                if (this.CWPM_CheckChar("=")) {
                                    return this.token.tokenType = CWPM_TokenType.NOT_CONGRUENT
                                }
                                return this.token.tokenType = CWPM_TokenType.NOT_EQUAL
                            }
                            return this.token.tokenType = CWPM_TokenType.NOT;
                        case "=":
                            if (this.CWPM_CheckChar("=")) {
                                if (this.CWPM_CheckChar("=")) {
                                    return this.token.tokenType = CWPM_TokenType.CONGRUENT
                                }
                                return this.token.tokenType = CWPM_TokenType.EQUAL
                            }
                            return this.token.tokenType = CWPM_TokenType.ASSIGNMENT;
                        case ">":
                            if (this.CWPM_CheckChar(">")) {
                                if (this.CWPM_CheckChar("=")) {
                                    return this.token.tokenType = CWPM_TokenType.BIT_RIGHT_ASSIGNMENT
                                }
                                return this.token.tokenType = CWPM_TokenType.BIT_RIGHT
                            }
                            this.CWPM_CheckChar("=");
                            return this.token.tokenType = CWPM_TokenType.GREATER;
                        case "<":
                            if (this.CWPM_CheckChar("=")) {
                                if (this.CWPM_CheckChar("=")) {
                                    return this.token.tokenType = CWPM_TokenType.CONGRUENT
                                }
                                return this.token.tokenType = CWPM_TokenType.EQUAL
                            }
                            this.CWPM_CheckChar("=");
                            return this.token.tokenType = CWPM_TokenType.LESS;
                        case "{":
                            return this.token.tokenType = CWPM_TokenType.OPENBRACE;
                        case "}":
                            return this.token.tokenType = CWPM_TokenType.CLOSEBRACE;
                        case "[":
                            return this.token.tokenType = CWPM_TokenType.OPENBRACKET;
                        case "]":
                            return this.token.tokenType = CWPM_TokenType.CLOSEBRACKET;
                        case "(":
                            return this.token.tokenType = CWPM_TokenType.OPENPAREN;
                        case ")":
                            return this.token.tokenType = CWPM_TokenType.CLOSEPAREN;
                        case "?":
                            return this.token.tokenType = CWPM_TokenType.QUESTION;
                        case ";":
                            return this.token.tokenType = CWPM_TokenType.SEMICOLON;
                        case ",":
                            return this.token.tokenType = CWPM_TokenType.COMMA;
                        case ":":
                            return this.token.tokenType = CWPM_TokenType.COLON;
                        case ".":
                            return this.token.tokenType = CWPM_TokenType.DOT;
                        case "~":
                            return this.token.tokenType = CWPM_TokenType.BIT_NOT;
                        default:
                            return this.token.tokenType = CWPM_TokenType.NOT.IDENTIFIED
                    }
                }
            }
        }
        this.token.tokenLength = this.currentPosition - this.token.tokenBeginPosition;
        return this.token.tokenType
    }
};
CWPM_JsParserData.prototype.CWPM_DetectOneToken = function() {
    var a = this.CWPM_GetOneToken();
    this.savedToken = this.token;
    this.token.tokenType = a;
    return a
};
CWPM_JsParserData.prototype.CWPM_CheckTokenType = function(a) {
    var b = this.CWPM_GetOneToken();
    if (b == a) {
        return true
    } else {
        this.savedToken = this.token;
        this.token.tokenType = b;
        return false
    }
};
CWPM_JsParserData.prototype.CWPM_JudgeIsRewriteProp = function(a) {
    var b = a.length;
    switch (b) {
        case 3:
            if (a == "src") {
                return "Src"
            } else {
                if (a == "URL") {
                    return "URL"
                } else {
                    if (a == "SRC") {
                        return "SRC"
                    }
                }
            }
            break;
        case 4:
            if (a == "hash") {
                return "Hash"
            } else {
                if (a == "host") {
                    return "Host"
                } else {
                    if (a == "href") {
                        return "Href"
                    }
                }
            }
            break;
        case 6:
            if (a == "action") {
                return "Action"
            } else {
                if (a == "cookie") {
                    return "Cookie"
                } else {
                    if (a == "cursor") {
                        return "Cursor"
                    } else {
                        if (a == "domain") {
                            return "Domain"
                        } else {
                            if (a == "filter") {
                                return "Filter"
                            } else {
                                if (a == "search") {
                                    return "Search"
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 7:
            if (a == "cssText") {
                return "CssText"
            } else {
                if (a == "BaseURL") {
                    return "BaseUrl"
                } else {
                    if (a == "DataURL") {
                        return "DataURL"
                    }
                }
            }
            break;
        case 8:
            if (a == "behavior") {
                return "Behavior"
            } else {
                if (a == "CodeBase") {
                    return "CodeBase"
                } else {
                    if (a == "hostname") {
                        return "Hostname"
                    } else {
                        if (a == "location") {
                            return "Location"
                        } else {
                            if (a == "pathname") {
                                return "Pathname"
                            } else {
                                if (a == "protocol") {
                                    return "Protocol"
                                } else {
                                    if (a == "referrer") {
                                        return "Referrer"
                                    }
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 9:
            if (a == "outerHTML") {
                return "OuterHTML"
            } else {
                if (a == "innerHTML") {
                    return "InnerHTML"
                }
            }
            break;
        case 10:
            if (a == "background") {
                return "Background"
            } else {
                if (a == "firstChild") {
                    return "FirstChild"
                } else {
                    if (a == "childNodes") {
                        return "ChildNodes"
                    }
                }
            }
            break;
        case 12:
            if (a == "DocumentHTML") {
                return "DocumentHTML"
            } else {
                if (a == "ReportSource") {
                    return "ReportSource"
                }
            }
            break;
        case 15:
            if (a == "backgroundImage") {
                return "BackgroundImage"
            }
            break;
        default:
            break
    }
    return -1
};
CWPM_JsParserData.prototype.CWPM_JudgeIsRewriteFunc = function(a) {
    var b = a.length;
    switch (b) {
        case 2:
            if (a == "go") {
                return "Go"
            }
            break;
        case 3:
            if (a == "add") {
                return "Add"
            }
            break;
        case 4:
            if (a == "open") {
                return "Open"
            } else {
                if (a == "attr") {
                    return "Attr"
                } else {
                    if (a == "eval") {
                        return "Eval"
                    } else {
                        if (a == "load") {
                            return "Load"
                        }
                    }
                }
            }
            break;
        case 5:
            if (a == "clear") {
                return "Clear"
            } else {
                if (a == "write") {
                    return "Write"
                } else {
                    if (a == "close") {
                        return "Close"
                    }
                }
            }
            break;
        case 6:
            if (a == "assign") {
                return "Assign"
            } else {
                if (a == "String") {
                    return "String"
                } else {
                    if (a == "reload") {
                        return "Reload"
                    }
                }
            }
            break;
        case 7:
            if (a == "refresh") {
                return "Refresh"
            } else {
                if (a == "replace") {
                    return "Replace"
                } else {
                    if (a == "loadurl") {
                        return "Loadurl"
                    } else {
                        if (a == "writeln") {
                            return "Writeln"
                        }
                    }
                }
            }
            break;
        case 8:
            if (a == "navigate") {
                return "Navigate"
            } else {
                if (a == "showHelp") {
                    return "ShowHelp"
                } else {
                    if (a == "AddParam") {
                        return "AddParam"
                    } else {
                        if (a == "toString") {
                            return "ToString"
                        }
                    }
                }
            }
            break;
        case 9:
            if (a == "navigate2") {
                return "Naviage2"
            }
            break;
        case 10:
            if (a == "setTimeout") {
                return "SetTimeout"
            } else {
                if (a == "initialize") {
                    return "Initialize"
                }
            }
            break;
        case 11:
            if (a == "addBehavior") {
                return "AddBehavior"
            } else {
                if (a == "appendChild") {
                    return "AppendChild"
                } else {
                    if (a == "execcommand") {
                        return "Execcommand"
                    } else {
                        if (a == "setInterval") {
                            return "SetInterval"
                        }
                    }
                }
            }
            break;
        case 12:
            if (a == "createObject") {
                return "CreateObject"
            } else {
                if (a == "EditDocument") {
                    return "EditDocument"
                } else {
                    if (a == "getAttribute") {
                        return "GetAttribute"
                    } else {
                        if (a == "setAttribute") {
                            return "SetAttribute"
                        } else {
                            if (a == "ViewDocument") {
                                return "ViewDocument"
                            }
                        }
                    }
                }
            }
            break;
        case 13:
            if (a == "createElement") {
                return "CreateElement"
            } else {
                if (a == "editDocument2") {
                    return "EditDocument2"
                } else {
                    if (a == "startDownload") {
                        return "StartDownload"
                    } else {
                        if (a == "SubmitChanges") {
                            return "SubmitChanges"
                        } else {
                            if (a == "viewDocument2") {
                                return "ViewDocument2"
                            } else {
                                if (a == "setAttribute") {
                                    return "SetAttribute"
                                }
                            }
                        }
                    }
                }
            }
            break;
        case 15:
            if (a == "showModalDialog") {
                return "ShowModalDialog"
            }
            break;
        case 16:
            if (a == "createStyleSheet") {
                return "CreateStyleSheet"
            }
            break;
        case 17:
            if (a == "CreateNewDocument") {
                return "CreateNewDocument"
            } else {
                if (a == "getResponseHeader") {
                    return "GetResponseHeader"
                }
            }
            break;
        case 18:
            if (a == "createNewDocument2") {
                return "CreateNewDocument2"
            } else {
                if (a == "insertAdjacentHTML") {
                    return "InsertAdjacentHTML"
                } else {
                    if (a == "showModelessDialog") {
                        return "ShowModelessDialog"
                    }
                }
            }
            break;
        case 21:
            if (a == "insertAdjacentElement") {
                return "InsertAdjacentElement"
            }
            break;
        case 24:
            if (a == "createContextualFragment") {
                return "CreateContextualFragment"
            }
            break;
        default:
            break
    }
    return -1
};
function CWPM_JsRewriteRecord(c, b, d, a) {
    this.insertPosition = c;
    this.insertString = b;
    this.insertLength = d;
    this.propItem = a
}
CWPM_JsParserData.prototype.CWPM_InsertText = function(d, a, c) {
    var b = new CWPM_JsRewriteRecord(d, c, c.length, a);
    this.rewritenToken[this.currentRewritenToken++] = b
};
CWPM_JsParserData.prototype.CWPM_DeleteText = function(a, b) {
    var c = new CWPM_JsRewriteRecord(a, 0, b, 0);
    this.rewritenToken[this.currentRewritenToken++] = c
};
function CWPM_GetDefaultParentName(a) {
    switch (a) {
        case "settimeout":
        case "setinterval":
        case "location":
        case "showmodaldialog":
        case "showmodelessdialog":
        case "open":
        case "show":
            return "window";
        case "action":
            return "parentNode";
        default:
            return null
    }
}
function CWPM_CheckHaveFunc(a) {
    switch (a) {
        case "open":
        case "close":
        case "write":
        case "writeln":
        case "setTimeout":
        case "addBehavior":
        case "insertAdjacentHTML":
        case "setInterval":
        case "showModalDialog":
        case "navigate":
        case "showModelessDialog":
        case "showHelp":
        case "postURL":
        case "getURL":
        case "reload":
        case "replace":
        case "assign":
        case "navigate":
        case "createStyleSheet":
            return 1;
        default:
            return 0
    }
}
CWPM_JsParserData.prototype.CWPM_RewriteMethod = function(j, l, h, c, k) {
    var g = 0;
    var b;
    var d = 0;
    var a = this.originalString.substr(c, k - c);
    var e = this.CWPM_JudgeIsRewriteFunc(a);
    do {
        var f = this.originalString.substr(k + d, 1);
        d++
    } while (" " == f);
    if (-1 == e || "(" != f) {
        return
    }
    if ("eval" == a) {
        this.methods[this.currentMethodIndex++] = this.CWPM_RewriteEval;
        return 1
    }
    if (0 > j) {
        if (0 == CWPM_CheckHaveFunc(a)) {
            return
        }
        this.methods[this.currentMethodIndex++] = h;
        this.CWPM_DeleteText(c, a.length);
        this.CWPM_InsertText(c, -h, "CWPM_API_Func_" + e + "(");
        this.CWPM_InsertText(k, -h, this.originalString.substr(c, k - c));
        return 1
    }
    g = c - 1;
    while (g > 0) {
        b = this.originalString.charAt(g);
        if (b != "." && !this.CWPM_IsWhiteSpace(b)) {
            break
        }
        g--
    }
    g++;
    this.methods[this.currentMethodIndex++] = h;
    methodStr = "CWPM_API_" + e + "(";
    if (this.returnPosition == j) {
        this.CWPM_InsertText(j, -h - 1, " ")
    }
    if (0 != this.withEndPosition && this.withEndPosition == j) {
        this.CWPM_InsertText(j, h, methodStr)
    } else {
        this.CWPM_InsertText(j, -h, methodStr)
    }
    this.CWPM_DeleteText(g, k - g);
    return 1
};
CWPM_JsParserData.prototype.CWPM_RewriteMethodBracket = function(h, m, l, c, d, g) {
    var j, e, f = 0, b, a = new Array();
    if (h < 0) {
        return 0
    }
    mFunc = this.originalString.substr(c + 1, d - c - 2);
    var k = this.CWPM_JudgeIsRewriteFunc(mFunc);
    if (-1 == k) {
        return 0
    }
    if ("eval" == mFunc) {
        this.methods[this.currentMethodIndex++] = this.CWPM_RewriteEval;
        return 1
    }
    f = m;
    this.methods[this.currentMethodIndex++] = l;
    tmp_str = "CWPM_API_" + k + "(";
    if (0 != this.withEndPosition && this.withEndPosition == h) {
        this.CWPM_InsertText(h, l, tmp_str)
    } else {
        this.CWPM_InsertText(h, -l, tmp_str)
    }
    this.CWPM_DeleteText(f, g - f);
    return 1
};
CWPM_JsParserData.prototype.CWPM_RewriteWriteproperty = function(g, j, c, a, d, h) {
    var f = 0;
    var b = this.originalString.substr(a, d - a);
    var e = this.CWPM_JudgeIsRewriteProp(b);
    if (0 > g) {
        if ("location" == b) {
            this.CWPM_InsertText(a, 0, "CWPM_API_Set_Location(window");
            this.CWPM_DeleteText(a, d - a);
            this.props[this.currentPropIndex++] = c;
            return this.CWPM_Rewrite
        } else {
            if ("src" == b) {
                return
            }
        }
        if (0 == this.rewriteWithFlag) {
            return this.CWPM_RewriteNo
        }
    }
    if ("Hash" == e || "Referrer" == e || "ChildNodes" == e) {
        return
    }
    if (-1 == e) {
        return
    }
    if (g < 0) {
        this.indexInWithState++;
        return this.CWPM_RewriteWith
    }
    f = j;
    this.props[this.currentPropIndex++] = c;
    tmpStr = "CWPM_API_Set_" + e + "(";
    if (0 != this.withEndPosition && this.withEndPosition == g) {
        this.CWPM_InsertText(g, c, tmpStr)
    } else {
        this.CWPM_InsertText(g, -c, tmpStr)
    }
    this.CWPM_DeleteText(f, d - f);
    return this.CWPM_Rewrite
};
CWPM_JsParserData.prototype.CWPM_RewriteWritepropertyBracket = function(k, m, c, a, f, b, d, j) {
    var l, h, e;
    if (k < 0) {
        return this.RewriteNo
    }
    l = this.originalString.substr(a + 1, f - a - 2);
    var g = this.CWPM_JudgeIsRewriteProp(l);
    if ("Hash" == g || "Referrer" == g || "ChildNodes" == g) {
        return
    }
    if (-1 == g) {
        return 1
    }
    h = m;
    this.props[this.currentPropIndex++] = c;
    e = "CWPM_API_Set_" + g + "(";
    if (0 != this.withEndPosition && this.withEndPosition == k) {
        this.CWPM_InsertText(k, c, e)
    } else {
        this.CWPM_InsertText(k, -c, e)
    }
    this.CWPM_DeleteText(h, j - h);
    return this.CWPM_Rewrite
};
CWPM_JsParserData.prototype.CWPM_RewriteReadProperty = function(h, l, g, d, c, j) {
    var b = this.originalString.substr(d, c - d);
    var e = this.CWPM_JudgeIsRewriteProp(b);
    if (h < 0) {
        return
    }
    if ("OuterHTML" == e) {
        return
    }
    if ("Location" == e) {
        var k = this.originalString.substr(c, 1);
        if ("." == k) {
            return
        }
        if (0 != this.parseWithFlag) {
            return
        }
    }
    if (-1 == e) {
        return
    }
    var f = l;
    var a = "CWPM_API_Get_" + e + "(";
    this.CWPM_InsertText(h, -g, a);
    this.CWPM_DeleteText(f, c - f);
    this.CWPM_InsertText(c, g, ")")
};
CWPM_JsParserData.prototype.CWPM_RewriteReadPropertyBracket = function(h, l, f, a, c, j, g) {
    if (h < 0) {
        return
    }
    var k = this.originalString.substr(a + 1, c - a - 2);
    var d = this.CWPM_JudgeIsRewriteProp(k);
    if (-1 == d || j) {
        return
    }
    var e = l;
    var b = "CWPM_API_Get_" + d + "(";
    this.CWPM_InsertText(h, -f, b);
    this.CWPM_DeleteText(e, g - e);
    this.CWPM_InsertText(g, f, ")")
};
CWPM_JsParserData.prototype.CWPM_GetCurrentWord = function() {
    if (0 == this.savedToken) {
        return this.jsString.substr(this.token.tokenBeginPosition, this.token.tokenLength)
    } else {
        return this.jsString.substr(this.savedToken.tokenBeginPosition, this.savedToken.tokenLength)
    }
};
CWPM_JsParserData.prototype.CWPM_AsEval = function(c, a, b) {
    if (a < 0) {
        this.currentMethodIndex--;
        return
    }
    var d = this.methods[--this.currentMethodIndex];
    if (d == this.CWPM_RewriteEval) {
        this.CWPM_InsertText(a + 1, -d, "CWPM_API_Func_Eval(");
        this.CWPM_InsertText(b, d, ")", 1)
    } else {
        if (c > 0) {
            this.CWPM_InsertText(a, d, ",")
        }
        this.CWPM_DeleteText(a, 1)
    }
};
CWPM_JsParserData.prototype.CWPM_ParseParameters = function(c) {
    var d;
    var a = 0;
    var b = this.token.tokenBeginPosition;
    this.setFlag(this.SCANNING);
    d = this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    this.unsetFlag(this.SCANNING);
    if (!d) {
        do {
            a++;
            if (!this.CWPM_AssignmentExpression()) {
                if (c) {
                    this.CWPM_AsEval(-1, -1, -1)
                }
                return 0
            }
        } while (this.CWPM_CheckTokenType(CWPM_TokenType.COMMA));
        this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN)
    }
    if (c) {
        this.CWPM_AsEval(a, b, this.CWPM_GetTokenBegin())
    }
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseEndOperators = function() {
    var b;
    this.setFlag(this.SCANNING);
    var a = this.CWPM_GetOneToken();
    this.unsetFlag(this.SCANNING);
    switch (a) {
        case 62:
            return this.CWPM_ParseFunctionStatement();
        case 3:
            if (this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE)) {
                return 1
            }
            do {
                a = this.CWPM_GetOneToken();
                switch (a) {
                    case 1:
                    case 2:
                    case 90:
                        break;
                    case 4:
                        return 1;
                    default:
                        return 0
                }
                this.CWPM_CheckTokenType(CWPM_TokenType.COLON);
                if (!this.CWPM_AssignmentExpression()) {
                    return 0
                }
            } while (this.CWPM_CheckTokenType(CWPM_TokenType.COMMA));
            this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE);
            return 1;
        case 5:
            this.setFlag(this.SCANNING);
            if (this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACKET)) {
                this.unsetFlag(this.SCANNING);
                return 1
            }
            this.unsetFlag(this.SCANNING);
            do {
                this.setFlag(this.SCANNING);
                a = this.CWPM_DetectOneToken();
                this.unsetFlag(this.SCANNING);
                if (a == CWPM_TokenType.CLOSEBRACKET) {
                    break
                }
                if (a != CWPM_TokenType.COMMA) {
                    this.CWPM_AssignmentExpression()
                }
            } while (this.CWPM_CheckTokenType(CWPM_TokenType.COMMA));
            this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACKET);
            return 1;
        case 7:
            b = this.CWPM_ParseSerialExpressions();
            if (!b) {
                return 0
            }
            this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
            return 1;
        case 1:
        case 2:
        case 90:
        case 64:
            return 1;
        default:
            return 0
    }
};
CWPM_JsParserData.prototype.CWPM_GetTokenBegin = function() {
    if (this.savedToken) {
        return this.savedToken.tokenBeginPosition
    }
    return this.currentPosition
};
CWPM_JsParserData.prototype.CWPM_ParsePropertyForObject = function(c) {
    var g = 0;
    var j = 0;
    var e;
    var r = 0;
    var h = 0;
    var q;
    var o = 0;
    var f = 0;
    var d = 0;
    this.setFlag(this.SCANNING);
    var q = this.CWPM_DetectOneToken();
    this.unsetFlag(this.SCANNING);
    h = this.CWPM_GetTokenBegin();
    switch (q) {
        case 44:
            this.CWPM_GetOneToken();
            if (!this.CWPM_ParsePropertyForObject(0)) {
                return 0
            }
            if (this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN)) {
                if (!this.CWPM_ParseParameters(0)) {
                    return 0
                }
            }
            break;
        case 90:
            g = 1;
        case 1:
        case 2:
        case 36:
            this.CWPM_GetOneToken();
            break;
        default:
            if (!this.CWPM_ParseEndOperators()) {
                return 0
            }
            break
    }
    for (j = 0; ; j++) {
        r = this.CWPM_GetJsBegin();
        q = this.CWPM_GetOneToken();
        if (q == CWPM_TokenType.END) {
            return 1
        }
        if (q == CWPM_TokenType.DOT) {
            q = this.CWPM_GetOneToken();
            f = this.token.tokenBeginPosition;
            this.CWPM_SaveToken(q);
            this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORDS_NO_FOUND);
            o = this.CWPM_GetTokenBegin();
            e = this.CWPM_DetectOneToken();
            if (e == CWPM_TokenType.ASSIGNMENT || e == CWPM_TokenType.ADD_ASSIGNMENT) {
                this.appendFlag = (e == CWPM_TokenType.ADD_ASSIGNMENT);
                this.rewriteFlag = this.CWPM_RewriteWriteproperty(h, r, j, f, o, this.appendFlag)
            } else {
                if (c && e == CWPM_TokenType.OPENPAREN) {
                    d = this.CWPM_RewriteMethod(h, r, j, f, o)
                } else {
                    this.CWPM_RewriteReadProperty(h, r, j, f, o, e == CWPM_TokenType.DOT)
                }
            }
        } else {
            if (CWPM_TokenType.OPENBRACKET == q) {
                var m = this.CWPM_DetectOneToken(), n = "", a = this.CWPM_GetTokenBegin(), l, p;
                if (m == CWPM_TokenType.STRING) {
                    n = this.CWPM_GetCurrentWord();
                    if (!this.CWPM_ParseSerialExpressions()) {
                        return 0
                    }
                    p = this.CWPM_GetJsBegin();
                    var k = this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACKET);
                    l = this.CWPM_GetJsBegin();
                    if (k) {
                        var b = this.originalString.substr(a, p - a);
                        if ((n != "") && (n == b.replace(/(^[\\s]*)|([\\s]*$)/g, ""))) {
                            e = this.CWPM_DetectOneToken();
                            if (e == CWPM_TokenType.ASSIGNMENT || e == CWPM_TokenType.ADD_ASSIGNMENT) {
                                this.appendFlag = (e == CWPM_TokenType.ADD_ASSIGNMENT);
                                this.rewriteFlag = this.CWPM_RewriteWritepropertyBracket(h, r, j, a, a + n.length, this.appendFlag, "", l)
                            } else {
                                if (c && e == CWPM_TokenType.OPENPAREN) {
                                    d = this.CWPM_RewriteMethodBracket(h, r, j, a, a + n.length, l)
                                } else {
                                    this.CWPM_RewriteReadPropertyBracket(h, r, j, a, a + n.length, e == CWPM_TokenType.DOT, l)
                                }
                            }
                        }
                    }
                } else {
                    if (!this.CWPM_ParseSerialExpressions()) {
                        return 0
                    }
                    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACKET)
                }
            } else {
                if (c && CWPM_TokenType.OPENPAREN == q) {
                    if (0 == j) {
                        if (this.CWPM_RewriteMethod(-1, -1, j, h, r)) {
                            d = 1
                        }
                    }
                    if (!this.CWPM_ParseParameters(d)) {
                        return 0
                    }
                    d = 0
                } else {
                    if (g && j == 0) {
                        if (q == CWPM_TokenType.ASSIGNMENT || q == CWPM_TokenType.ADD_ASSIGNMENT) {
                            this.appendFlag = (q == CWPM_TokenType.ADD_ASSIGNMENT);
                            this.rewriteFlag = this.CWPM_RewriteWriteproperty(-1, -1, j, h, r, this.appendFlag)
                        } else {
                            this.CWPM_RewriteReadProperty(-1, -1, j, h, r, 0)
                        }
                    }
                    this.CWPM_SaveToken(q);
                    return 1
                }
            }
        }
    }
    return 0
};
CWPM_JsParserData.prototype.CWPM_ParseObject = function() {
    this.setFlag(this.SCANNING);
    var a = this.CWPM_GetOneToken();
    this.unsetFlag(this.SCANNING);
    switch (a) {
        case 27:
        case 14:
        case 15:
        case 65:
        case 60:
            return this.CWPM_ParseObject();
        case 266:
        case 267:
            return this.CWPM_ParsePropertyForObject(1);
        default:
            break
    }
    this.CWPM_SaveToken(a);
    if (!this.CWPM_ParsePropertyForObject(1)) {
        return 0
    }
    a = this.CWPM_DetectOneToken();
    if (a == CWPM_TokenType.DADD || a == CWPM_TokenType.DSUBTRACT) {
        this.CWPM_GetOneToken()
    }
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseExpressions = function() {
    var b = 1;
    for (; ; ) {
        if (b) {
            b = 0
        } else {
            if (CWPM_TokenType.END == this.CWPM_GetOneToken()) {
                return 0
            }
        }
        if (!this.CWPM_ParseObject()) {
            return 0
        }
        var a = this.CWPM_DetectOneToken();
        switch (a) {
            case CWPM_TokenType.ADD:
            case CWPM_TokenType.SUBTRACT:
            case CWPM_TokenType.MUTIPLY:
            case CWPM_TokenType.DIVIDE:
            case CWPM_TokenType.MODULO:
            case CWPM_TokenType.BIT_LEFT:
            case CWPM_TokenType.BIT_RIGHT:
            case CWPM_TokenType.GREATER:
            case CWPM_TokenType.LESS:
            case CWPM_TokenType.EQUAL:
            case CWPM_TokenType.NOT_EQUAL:
            case CWPM_TokenType.CONGRUENT:
            case CWPM_TokenType.NOT_CONGRUENT:
            case CWPM_TokenType.BIT_AND:
            case CWPM_TokenType.BIT_OR:
            case CWPM_TokenType.BIT_AND_OR:
            case CWPM_TokenType.LOGICAL_AND:
            case CWPM_TokenType.LOGICAL_OR:
                continue;
            case CWPM_TokenType.QUESTION:
                this.CWPM_GetOneToken();
                b = this.CWPM_AssignmentExpression();
                if (!b) {
                    return 0
                }
                this.CWPM_CheckTokenType(CWPM_TokenType.COLON);
                return this.CWPM_AssignmentExpression();
            default:
                return 1
        }
    }
};
CWPM_JsParserData.prototype.CWPM_AssignmentAppend = function(c, b, a) {
    var d = this.props[--this.currentPropIndex];
    this.CWPM_InsertText(c, d, ",");
    if (a) {
        this.CWPM_DeleteText(c, 2);
        this.CWPM_InsertText(b, d, ",true)")
    } else {
        this.CWPM_DeleteText(c, 1);
        this.CWPM_InsertText(b, d, ",false)")
    }
};
CWPM_JsParserData.prototype.CWPM_IsWhiteSpace = function(a) {
    return a == " " || a == "\t" || a == "\r" || a == "\n"
};
CWPM_JsParserData.prototype.CWPM_RewriteReadInWith = function(h, j, k, e, b) {
    if (!this.rewriteWithFlag) {
        return
    }
    if (h == -1 || j == -1) {
        return
    }
    while (this.CWPM_IsWhiteSpace(this.originalString.charAt(h))) {
        ig_ws_begin_with++
    }
    while (this.CWPM_IsWhiteSpace(this.originalString.charAt(j - 1))) {
        j--
    }
    var f = "";
    var d = k - h;
    var c = j - h;
    if (c <= 0) {
        return
    }
    var a = this.originalString.substr(h, c);
    var g = this.CWPM_JudgeIsRewriteProp(a);
    if (-1 == g) {
        return
    }
    this.CWPM_InsertText(h, this.currentIndex, "((CWPM_WithTmpVar =");
    f = "), (CWPM_WithTmpElement=CWPM_API_PropertyDefined('" + a + "',     " + this.rewriteWithFlag + ", CWPM_WithArray)),";
    f += "(CWPM_WithTmpElement";
    f += "? CWPM_API_Set_" + g + "(CWPM_WithTmpElement, CWPM_WithTmpVar, " + (b ? true : false) + ")";
    f += ": " + a + " " + (b ? "+=" : "=") + " CWPM_WithTmpVar))";
    this.CWPM_InsertText(e, this.currertIndex, f);
    this.CWPM_DeleteText(h, d);
    this.indexInWithState--
};
CWPM_JsParserData.prototype.CWPM_AssignmentExpression = function() {
    var d;
    var e;
    var a = -1;
    var b = -1;
    if (this.parseWithFlag) {
        this.setFlag(this.SCANNING);
        d = this.CWPM_GetOneToken();
        a = this.token.tokenBeginPosition;
        e = this.token.tokenLength;
        this.CWPM_SaveToken(d);
        this.unsetFlag(this.SCANNING);
        b = this.token.tokenBeginPosition + e
    }
    if (!this.CWPM_ParseExpressions()) {
        return 0
    }
    if (!this.CWPM_CheckTokenType(CWPM_TokenType.ASSIGNMENT) && !this.CWPM_CheckTokenType(CWPM_TokenType.ADD_ASSIGNMENT)) {
        return 1
    }
    var h = this.rewriteFlag;
    this.rewriteFlag = this.CWPM_RewriteNo;
    var f = this.appendFlag;
    var g = this.token.tokenBeginPosition;
    var c = this.CWPM_GetJsBegin();
    if (!this.CWPM_AssignmentExpression()) {
        if (this.CWPM_Rewrite == h) {
            this.CWPM_AssignmentAppend(-1, -1, -1)
        }
        return 0
    }
    if (this.CWPM_Rewrite == h) {
        this.CWPM_AssignmentAppend(g, this.CWPM_GetJsBegin(), f)
    } else {
        if (this.CWPM_RewriteWith == h) {
            this.CWPM_RewriteReadInWith(a, b, c, this.CWPM_GetJsBegin(), f)
        }
    }
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseVarExpression = function() {
    do {
        this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORDS_NO_FOUND);
        if (this.CWPM_CheckTokenType(CWPM_TokenType.ASSIGNMENT)) {
            if (!this.CWPM_AssignmentExpression()) {
                return 0
            }
        }
    } while (this.CWPM_CheckTokenType(CWPM_TokenType.COMMA));
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseSerialExpressions = function() {
    do {
        if (!this.CWPM_AssignmentExpression()) {
            return 0
        }
    } while (this.CWPM_CheckTokenType(CWPM_TokenType.COMMA));
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseForStatement = function() {
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    var a = this.CWPM_DetectOneToken();
    if (CWPM_TokenType.KEYWORD_VAR == a) {
        this.CWPM_ParseVarExpression()
    } else {
        if (a != CWPM_TokenType.SEMICOLON) {
            this.CWPM_ParseSerialExpressions()
        }
    }
    if (a != CWPM_TokenType.SEMICOLON && this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORD_IN)) {
        this.CWPM_ParseSerialExpressions()
    } else {
        this.CWPM_CheckTokenType(CWPM_TokenType.SEMICOLON);
        if (this.CWPM_DetectOneToken() != CWPM_TokenType.SEMICOLON) {
        }
        this.CWPM_ParseSerialExpressions();
        this.CWPM_CheckTokenType(CWPM_TokenType.SEMICOLON);
        if (this.CWPM_DetectOneToken() != CWPM_TokenType.CLOSEPAREN) {
        }
        this.CWPM_ParseSerialExpressions()
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    return this.CWPM_ParseUtilStatementEnd()
};
CWPM_JsParserData.prototype.CWPM_SaveToken = function(a) {
    this.savedToken = this.token;
    this.savedToken.tokenType = a
};
CWPM_JsParserData.prototype.CWPM_ParseFunctionStatement = function() {
    this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORDS_NO_FOUND);
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    if (!this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN)) {
        do {
            this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORDS_NO_FOUND)
        } while (this.CWPM_CheckTokenType(CWPM_TokenType.COMMA));
        this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN)
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENBRACE);
    this.CWPM_Parser();
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE);
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseIfStatement = function() {
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    if (!this.CWPM_ParseSerialExpressions()) {
        return 0
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    if (!this.CWPM_ParseUtilStatementEnd()) {
        return 0
    }
    if (this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORD_ELSE)) {
        this.CWPM_ParseUtilStatementEnd()
    }
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseDoStatement = function() {
    if (!this.CWPM_ParseUtilStatementEnd()) {
        return 0
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORD_WHILE);
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    if (!this.CWPM_ParseSerialExpressions()) {
        return 0
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseWhileStatement = function() {
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    if (!this.CWPM_ParseSerialExpressions()) {
        return 0
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    return this.CWPM_ParseUtilStatementEnd()
};
CWPM_JsParserData.prototype.CWPM_ParseSwitchStatement = function() {
    var a;
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    if (!this.CWPM_ParseSerialExpressions()) {
        return 0
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENBRACE);
    while ((a = this.CWPM_GetOneToken()) != CWPM_TokenType.CLOSEBRACE) {
        if (a == CWPM_TokenType.END) {
            return 0
        }
        if (a == CWPM_TokenType.KEYWORD_CASE) {
            this.CWPM_ParseSerialExpressions()
        }
        this.CWPM_CheckTokenType(CWPM_TokenType.COLON);
        for (; ; ) {
            a = this.CWPM_DetectOneToken();
            if (a == CWPM_TokenType.CLOSEBRACE || a == CWPM_TokenType.KEYWORD_CASE || a == CWPM_TokenType.KEYWORD_DEFALT || a == CWPM_TokenType.END) {
                break
            }
            if (!this.CWPM_ParseUtilStatementEnd()) {
                return 0
            }
        }
    }
    return 1
};
CWPM_JsParserData.prototype.CWPM_ParseTryStatement = function() {
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENBRACE);
    this.CWPM_Parser();
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE);
    while (CWPM_TokenType.KEYWORD_CATCH == this.CWPM_DetectOneToken()) {
        this.CWPM_GetOneToken();
        this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
        this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORDS_NO_FOUND);
        if (this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORD_IF)) {
            this.CWPM_ParseSerialExpressions()
        }
        this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
        this.CWPM_CheckTokenType(CWPM_TokenType.OPENBRACE);
        this.CWPM_Parser();
        this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE)
    }
    if (this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORD_FINALLY)) {
        this.CWPM_CheckTokenType(CWPM_TokenType.OPENBRACE);
        this.CWPM_Parser();
        this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE)
    }
    return 1
};
CWPM_JsParserData.prototype.CWPM_GetJsBegin = function() {
    if (this.savedToken) {
        return this.savedToken.jsBeginPosition
    } else {
        return this.token.tokenBeginPosition + this.token.tokenLength
    }
};
CWPM_JsParserData.prototype.CWPM_RewriteWithObject = function(b) {
    this.rewriteWithFlag++;
    var a = "CWPM_WithArray[" + (this.rewriteWithFlag - 1) + "]=";
    this.CWPM_InsertText(b, this.rewriteWithFlag, a)
};
CWPM_JsParserData.prototype.CWPM_RewriteWithFunction = function(b, c) {
    this.rewriteWithFlag--;
    if (this.rewriteWithFlag == 0) {
        var a = "{var CWPM_WithArray=new Array();";
        var d = b - 1;
        while (d > 0) {
            var f = this.originalString.charAt(d);
            if (!this.CWPM_IsWhiteSpace(f)) {
                break
            }
            d--
        }
        d++;
        var e = this.originalString.substr(d, b - d);
        a += e;
        this.CWPM_InsertText(b, this.rewriteWithFlag, a);
        this.CWPM_InsertText(c, 0, "}");
        this.withEndPosition = c
    }
};
CWPM_JsParserData.prototype.CWPM_ParseWithStatement = function() {
    this.parseWithFlag++;
    var a = this.token.tokenBeginPosition;
    this.CWPM_CheckTokenType(CWPM_TokenType.OPENPAREN);
    var c = this.CWPM_GetJsBegin();
    this.CWPM_ParseSerialExpressions();
    this.CWPM_RewriteWithObject(c);
    this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEPAREN);
    var b = this.CWPM_ParseUtilStatementEnd();
    var d = this.CWPM_GetJsBegin();
    this.CWPM_RewriteWithFunction(a, d);
    this.parseWithFlag--;
    return b
};
CWPM_JsParserData.prototype.CWPM_ParseUtilStatementEnd = function() {
    this.setFlag(this.SCANNING);
    var a = this.CWPM_GetOneToken();
    this.unsetFlag(this.SCANNING);
    switch (a) {
        case 62:
            if (!this.CWPM_ParseFunctionStatement()) {
                return 0
            }
            break;
        case 41:
            if (!this.CWPM_ParseIfStatement()) {
                return 0
            }
            return 1;
        case 46:
            this.CWPM_ParseVarExpression();
            break;
        case 3:
            this.CWPM_Parser();
            this.CWPM_CheckTokenType(CWPM_TokenType.CLOSEBRACE);
            return 1;
        case 43:
            if (!this.CWPM_ParseForStatement()) {
                return 0
            }
            return 1;
        case 40:
            if (!this.CWPM_ParseDoStatement()) {
                return 0
            }
            break;
        case 54:
            if (!this.CWPM_ParseWhileStatement()) {
                return 0
            }
            return 1;
        case 58:
            if (!this.CWPM_ParseSwitchStatement()) {
                return 0
            }
            return 1;
        case 45:
            if (!this.CWPM_ParseTryStatement()) {
                return 0
            }
            return 1;
        case 53:
            if (!this.CWPM_ParseSerialExpressions()) {
                return 0
            }
            break;
        case 49:
            return this.CWPM_ParseWithStatement();
        case 57:
            this.returnFlag = 1;
            this.returnPosition = this.currentPosition;
            this.setFlag(this.SCANNING | this.CASCADING);
            a = this.CWPM_DetectOneToken();
            this.unsetFlag(this.SCANNING | this.CASCADING);
            if (a != CWPM_TokenType.END && a != CWPM_TokenType.SEMICOLON && a != CWPM_TokenType.CLOSEBRACE) {
                this.CWPM_ParseSerialExpressions()
            }
            this.returnFlag = 0;
            break;
        case 50:
        case 61:
            this.setFlag(this.CASCADING);
            this.CWPM_CheckTokenType(CWPM_TokenType.KEYWORDS_NO_FOUND);
            this.unsetFlag(this.CASCADING);
            break;
        default:
            this.CWPM_SaveToken(a);
            this.CWPM_ParseSerialExpressions();
            if (this.CWPM_CheckTokenType(CWPM_TokenType.COLON)) {
                return this.CWPM_ParseUtilStatementEnd()
            }
            break
    }
    this.CWPM_CheckTokenType(CWPM_TokenType.SEMICOLON);
    return 1
};
CWPM_JsParserData.prototype.CWPM_Parser = function() {
    var a;
    for (; ; ) {
        this.setFlag(this.SCANNING);
        a = this.CWPM_DetectOneToken();
        this.unsetFlag(this.SCANNING);
        if (a == CWPM_TokenType.END || a == CWPM_TokenType.CLOSEBRACE || a == CWPM_TokenType.NOT_IDENTIFIED) {
            return
        }
        this.CWPM_ParseUtilStatementEnd()
    }
};
CWPM_RewriterRecordCmp = function(b, a) {
    if (b.insertPosition < a.insertPosition) {
        return -1
    } else {
        if (b.insertPosition > a.insertPosition) {
            return 1
        } else {
            if (b.propItem < a.propItem) {
                return -1
            } else {
                if (b.propItem > a.propItem) {
                    return 1
                }
            }
        }
    }
    return 0
};
CWPM_JsParserData.prototype.CWPM_Rewriter = function() {
    var a = "";
    var b = 0;
    var c = 0;
    if (0 == this.rewritenToken.length) {
        return this.originalString
    }
    this.rewritenToken.sort(CWPM_RewriterRecordCmp);
    for (var d = 0; d < this.rewritenToken.length; d++) {
        c = this.rewritenToken[d].insertPosition - b;
        if (c > 0) {
            a += this.originalString.substr(b, c);
            b += c
        }
        if (this.rewritenToken[d].insertString == 0 && "string" != typeof (this.rewritenToken[d].insertString)) {
            b += this.rewritenToken[d].insertLength
        } else {
            a += this.rewritenToken[d].insertString
        }
    }
    var e = this.originalString.length - b;
    if (e > 0) {
        a += this.originalString.substr(b, e)
    }
    return a
};
function CWPM_Js(a) {
    if (typeof (a) != "string") {
        return a
    }
    if (a.length < 30 && a.indexOf("window") < 0 && a.indexOf("document") < 0 && a.indexOf("location") && a.indexOf("innerHTML") < 0 && a.indexOf("outerHTML") && a.indexOf("insertAdjacentHTML") < 0 && a.indexOf("src") && a.indexOf("open") < 0 && a.indexOf("eval")) {
        return a
    }
    var b = new CWPM_JsParserData(a);
    b.CWPM_Parser();
    return b.CWPM_Rewriter()
}
function CWPM_API_Func_Eval(a) {
    return CWPM_Js(a)
}
var g_jsonDoc = {cwpmDoc: null,DSPopupDoc: null};
var g_CwpmInfoCached = false;
var g_cwpmInfo = null;
var g_cwpmUrlRegx = /^\s*(http(s?):\/\/([^\/]*:[^\/]*@)?([a-z0-9_.-]*)(:[0-9]*)?)?(.*)/i;
var g_cwpmProtocolInfo = null;
var g_cwpmUrl = "";
var g_url = window.document.location.href;
var g_CwpmCookie = "";
var g_CwpmLocStr = "";
var g_CwpmLocObj = "";
var g_CwpmIframeUsage = -1;
var g_HSTBSettings = 0;
var g_CwpmFakeProtocol = 0;
var g_SVNaxflag = 0;
var hs_checkIEVersion = CWPM_Check_IEVersion(6);
var g_SVNCache = new Object();
var g_CwpmTimeoutCtx = 0;
var g_CwpmTrivialWrite = 0;
var g_SVNSaved = "";
var g_IsSet = false;
var g_AttrRegx = /^\s*(=|[^>=\s]+)\s*(=\s*(\'[^\']*\'|\"[^\"]*\"|`[^`]*`|[^\'\">\s][^>\s]*))?/;
var g_TagRegx = /^<([\/]?[^\s\/><]*)(\s*)((\s|=|[^<>=\s]+(\s*?=\s*?(\'[^\']*\'|\"[^\"]*\"|`[^`]*`|[^=\'\"<>\s][^\'\"<>\s]*))?)*)(.)/;
var g_CwpmFirstScript;
var g_SVNUseCache = 0;
var g_isBaseSet = false;
var g_canBaseSet = true;
var CwpmActiveXParamsData = "var CwpmActiveXParams = [[ '5220CB21-C88D-11CF-B347-00AA00A28331',   [     ['LPKPath', 1]   ]],[ '36299202-09EF-4ABF-ADB9-47C599DBE778',   [     ['_ExtentX', 3]   ]],[ '3BFFE033-BF43-11D5-A271-00A024A51325',   [     ['General_ServerName', 2],    ['General_URL', 1]   ]],[ '81B75A1B-80FC-40D8-BCB4-EB6DC7A8ACFF',   [     ['h', 2],    ['xobj', 1]   ]],[ '5BDBA960-6534-11D3-97C7-00500422B550',   [     ['FullUrl', 1]   ]],[ '00120000-B1BA-11CE-ABC6-F5B2E79D9E3F',   [     ['BitmapDataPath', 1]   ]],[ '00130000-B1BA-11CE-ABC6-F5B2E79D9E3F',   [     ['BitmapDataPath', 1]   ]],[ '05D96F71-87C6-11D3-9BE4-00902742D6E0',   [     ['General_ServerName', 2],    ['General_URL', 1]   ]],[ 'D27CDB6E-AE6D-11CF-96B8-444553540000',   [     ['movie', 1],    ['quality', 1]   ]],[ '65BCBEE4-7728-41A0-97BE-14E1CAE36AAE',   [     ['ListWeb', 6],    ['ListData', 7]   ]],[ '983A9C21-8207-4B58-BBB8-0EBC3D7C5505',   [     ['General_ServerName', 2],    ['General_URL', 1]   ]],[ '1E2941E3-8E63-11D4-9D5A-00902742D6E0',   [     ['General_ServerName', 2],    ['General_URL', 1]   ]],[ 'ADB880A6-D8FF-11CF-9377-00AA003B7A11',   [     ['Item1', 1]   ]],[ '75AA409D-05F9-4F27-BD53-C7339D4B1D0A',   [     ['General_ServerName', 2],    ['General_URL', 1]   ]],[ 'E008A543-CEFB-4559-912F-C27C2B89F13B',   [     ['General_ServerName', 2],    ['General_URL', 1]   ]],[ 'DCB98BE9-88EE-4AD0-9790-2B169E8D5BBB',   [     ['_Version', 1]   ]],];var CWPMJVMClassIds = ['CAFEEFAC-0014-0000-0000-ABCDEFFEDCBA','CAFEEFAC-0014-0001-0000-ABCDEFFEDCBA','CAFEEFAC-0012-0002-0000-ABCDEFFEDCBA','CAFEEFAC-0012-0003-0000-ABCDEFFEDCBA','CAFEEFAC-0015-0000-0004-ABCDEFFEDCBA','CAFEEFAC-0014-0002-0006-ABCDEFFEDCBA','8AD9C840-044E-11D1-B3E9-00805F499D93','CAFEEFAC-0012-0000-0000-ABCDEFFEDCBA','CAFEEFAC-0013-0000-0000-ABCDEFFEDCBA',];";
eval(CwpmActiveXParamsData);
function CWPM_API_Native(b) {
    if (b) {
        if (b instanceof Function) {
            return true
        }
        var a = b + "";
        if (a) {
            if (navigator != null && typeof (navigator) == "object" && typeof (navigator.userAgent) != "undefined") {
                if (a.match(/^\s*function[^{]*\{\s*\[\s*native\s*code\s*\]\s*\}\s*$/)) {
                    return true
                }
                if (navigator.userAgent.match(/Safari/)) {
                    if (a.match(/^\s*\[function\]\s*$/)) {
                        return true
                    }
                }
                return false
            } else {
                return false
            }
        } else {
            if (typeof (b) == "object" && typeof (b.constructor) == "undefined") {
                return true
            }
        }
    }
    return false
}
function CWPM_API_PropertyDefined(b, d, a) {
    if (b == "open") {
        d = 1
    }
    for (var c = d; c > 0; c--) {
        if (typeof (a[c - 1][b]) != "undefined") {
            return a[c - 1]
        }
    }
    return 0
}
function CWPM_API_AddHierarchy(b, a) {
    if (4 == arguments.length && CWPM_StrEqual(b.className, "treeFolder") && CWPM_StrEqual(b.id, "objTree")) {
        arguments[2] = CWPM_Url(a)
    }
    if (4 == arguments.length) {
        return b.addHierarchy(argument[1], argument[2], argument[3])
    }
    if (3 == arguments.length) {
        return b.addHierarchy(argument[1], argument[2])
    }
    return b.addHierarchy(argument[1])
}
function CWPM_StrEqual(b, a) {
    if (typeof (b) != "string" || typeof (a) != "string") {
        return false
    }
    return b == a
}
function CWPM_API_Go(c, b) {
    var a = CWPM_Get_Arguments_Array("go", arguments, true);
    if (2 == arguments.length && "string" == typeof (b) && (c == window.history || c == top.window.history)) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Add(f, c, b) {
    var a;
    if ("object" == typeof (c) && "object" == typeof (f) && "undefined" != f.selectedIndex && 3 == arguments.length) {
        if (null == navigator.userAgent.match("MSIE")) {
            a = new Array("add", f, c, b)
        } else {
            a = new Array("add", f, c, b)
        }
        try {
            return CWPM_Call_Method(a)
        } catch (d) {
            return arguments[0].add(arguments[1], arguments[2])
        }
    }
    a = CWPM_Get_Arguments_Array("add", arguments, true);
    return CWPM_Call_Method(a)
}
function CWPM_Is_XmLElement(a) {
    return null != a && "number" == typeof (a.length) && "unknown" == typeof (a.namespaceURI) && "unknown" == typeof (a.add)
}
function CWPM_API_Attr(e, b, d) {
    var c = b;
    if ("string" == typeof (b)) {
        c = b.toLowerCase()
    }
    if ("string" == typeof (b) && "string" == typeof (d) && e && e.attr("tagName") && "string" == typeof (e.attr("tagName")) && (("form" == e.attr("tagName").toLowerCase() && "action" == c) || ("img" == e.attr("tagName").toLowerCase() && "src" == c))) {
        arguments[2] = CWPM_Url(d)
    } else {
        if (e && "number" == typeof (e.length) && 0 < e.length && (e.jquery || ("object" == typeof (e.fn) && "string" == typeof (e.fn.jquery)))) {
            if ("undefined" == typeof (d)) {
                if ("object" == typeof (arguments[1])) {
                    if ("string" == typeof (arguments[1].background) && arguments[1].background.match(/url\(\s*/i)) {
                        arguments[1].background = CWPM_CSS_Url(arguments[1].background)
                    }
                    var a = CWPM_Get_Arguments_Array("attr", arguments, true);
                    return CWPM_Call_Method(a)
                } else {
                    if ("string" == typeof (arguments[1])) {
                        if (2 == arguments.length) {
                            d = e.attr(arguments[1])
                        } else {
                            if (4 == arguments.length) {
                                d = e.attr(arguments[1], arguments[2], arguments[3])
                            }
                        }
                        if ("string" == typeof (d)) {
                            if ("background" == c || "backgroundImage" == c || "background-image" == c) {
                                d = CWPM_CSS_Origin_Url(d)
                            } else {
                                if ("href" == c || "action" == c || "src" == c) {
                                    d = CWPM_Origin_Url(d)
                                }
                            }
                        }
                        return d
                    }
                }
            }
            if (3 == arguments.length && (arguments[2] instanceof Function)) {
                return e.attr(arguments[1], arguments[2])
            }
            if ("string" == typeof (b)) {
                if ("background" == c || "backgroundImage" == c || "background-image" == c) {
                    arguments[2] = CWPM_CSS_Url(d)
                } else {
                    if ("href" == c || "action" == c || "src" == c) {
                        arguments[2] = CWPM_Url(d)
                    }
                }
            }
        }
    }
    var a = CWPM_Get_Arguments_Array("attr", arguments, true);
    return CWPM_Call_Method(a)
}
function CWPM_API_Load(c, b) {
    var a = CWPM_Get_Arguments_Array("load", arguments, true);
    if ("undefined" == typeof (b)) {
        return c.load()
    }
    if (arguments.length > 3) {
        return CWPM_Call_Method(a)
    }
    if ("string" == typeof (b)) {
        if (CWPM_XmlParser(c) || (CWPM_Check_Xml(c)) || (CWPM_Check_Xml(c) && "undefined" == typeof (c.id)) || CWPM_Check_Xml(c) || (CWPM_Xml_Doc(c) || CWPM_Check_Mime(c))) {
            a[2] = CWPM_Url(b)
        } else {
            if (CWPM_Check_Func(b)) {
                return c.load(arguments[1])
            } else {
                if (b.match(/s_XSLTViewEntries/)) {
                    if (-1 == navigator && null == navigator.userAgent.match(/IE/) && "boolean" == typeof (c.async)) {
                        a[2] = CWPM_Url(b)
                    }
                } else {
                    if (CWPM_Check_Func(arguments[2]) && 3 == arguments.length) {
                        a[2] = CWPM_Url(b)
                    }
                }
            }
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_Load(a) {
    if (CWPM_Check_Func(arguments[0])) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arr)
}
function CWPM_API_Open(g, d, e, c, q, p) {
    var j = CWPM_Get_Arguments_Array("open", arguments, true);
    if (CWPM_Check_Win(g)) {
        if ("" == d) {
            if (CWPM_Is_Blank_Win(e)) {
                d = "about:blank"
            }
        } else {
            d = CWPM_Url(d)
        }
        j[2] = d;
        var r = CWPM_Call_Method(j);
        CWPM_Store_Opened_Win(r);
        return r
    }
    if (CWPM_Check_XmlHttp(g)) {
        if (e instanceof Object) {
            if ("string" == typeof (e.url)) {
                e.url = CWPM_Url(e.url)
            }
            return CWPM_Call_Method(j)
        }
        if (e.match(/^http:\/behind/i)) {
            var h = "http://" + CWPM_API_Get_Host(location);
            e = e.replace("http:", h);
            return g.open(d, CWPM_Url(e), c)
        }
        if ("baike.baidu.com" == CWPM_API_Get_Domain(window.document)) {
            j[3] = CWPM_Url(e, 1);
            return CWPM_Call_Method(j)
        }
        j[3] = CWPM_Url(e);
        return CWPM_Call_Method(j)
    }
    if (CWPM_Check_Request(g)) {
        var b = document.cookie;
        var n = "DSID=";
        var f = b.indexOf("; " + n);
        var m = 1;
        var k = "";
        if (-1 == f) {
            f = b.indexOf(n);
            if (-1 == f) {
                m = 0
            }
        } else {
            f += 2
        }
        if (m) {
            var a = document.cookie.indexOf(";", f);
            if (-1 == a) {
                a = b.length
            }
            k = b.substring(f + n.length, a)
        }
        j[3] = CWPM_Url(e);
        return CWPM_Call_Method(j)
    }
    if (CWPM_Check_Xml(g)) {
        return g.open(d, CWPM_Url(e), c)
    }
    if (CWPM_Check_Doc(g)) {
        var l = false;
        var o;
        if ("undefined" != typeof (g.DSPopupDoc)) {
            l = true;
            o = g.DSPopupDoc
        }
        var r;
        if (2 <= arguments.length) {
            if ("" == d) {
                if (CWPM_Is_Blank_Win(e)) {
                    d = "about:blank"
                }
            } else {
                if ("string" == typeof (d)) {
                    d = CWPM_Url(d)
                }
            }
            j[2] = d;
            r = CWPM_Call_Method(j);
            CWPM_Store_Opened_Win(r)
        }
        if (l) {
            g.DSPopupDoc = o
        }
        if (2 <= arguments.length) {
            return r
        }
    } else {
        if (CWPM_OfficeApp(g)) {
            j[2] = CWPM_CwpmInfo_Before(d, true)
        }
    }
    return CWPM_Call_Method(j)
}
function CWPM_API_ActiveXOpen(h, e, f, d, q, p) {
    var j = CWPM_Get_Arguments_Array("Open", arguments, true);
    if (CWPM_Check_Win(h)) {
        if ("" == e) {
            if (CWPM_Is_Blank_Win(f)) {
                e = "about:blank"
            }
        } else {
            e = CWPM_Url(e)
        }
        j[2] = e;
        var r = CWPM_Call_Method(j);
        CWPM_Store_Opened_Win(r);
        return r
    }
    if (CWPM_Check_XmlHttp(h)) {
        if (f.match(/^http:\/behind\/adtest.aspx/i)) {
            return h.Open(e, f, d)
        }
        return h.Open(e, CWPM_Url(f), d)
    }
    if (CWPM_Check_Request(h)) {
        var c = document.cookie;
        var n = "DSID=";
        var g = c.indexOf("; " + n);
        var m = 1;
        var k = "";
        if (-1 == g) {
            g = c.indexOf(n);
            if (-1 == g) {
                m = 0
            }
        } else {
            g += 2
        }
        if (m) {
            var b = document.cookie.indexOf(";", g);
            if (-1 == b) {
                b = c.length
            }
            k = c.substring(g + n.length, b)
        }
        var a = CWPM_Url(f);
        return h.open(e, a, d, q, p)
    }
    if (CWPM_Check_Xml(h)) {
        return h.open(e, CWPM_Url(f), d)
    }
    if (CWPM_Check_Doc(h)) {
        var l = false;
        var o;
        if ("undefined" != typeof (h.DSPopupDoc)) {
            l = true;
            o = h.DSPopupDoc
        }
        var r;
        if (2 <= arguments.length) {
            if ("" == e) {
                if (CWPM_Is_Blank_Win(f)) {
                    e = "about:blank"
                }
            } else {
                if ("string" == typeof (e)) {
                    e = CWPM_Url(e)
                }
            }
            j[2] = e;
            r = CWPM_Call_Method(j);
            CWPM_Store_Opened_Win(r)
        }
        if (l) {
            h.DSPopupDoc = o
        }
        if (2 <= arguments.length) {
            return r
        }
    } else {
        if (CWPM_OfficeApp(h)) {
            j[2] = CWPM_CwpmInfo_Before(e, true)
        }
    }
    return CWPM_Call_Method(j)
}
function CWPM_API_Func_Open(c, b) {
    if (CWPM_Check_Func(arguments[0])) {
        if ("" == arguments[1]) {
            if (CWPM_Is_Blank_Win(argumens[2])) {
                arguments[1] = "about:blank"
            }
        } else {
            arguments[1] = CWPM_Url(arguments[1])
        }
    }
    var a = CWPM_Call_Func(arguments);
    CWPM_Store_Opened_Win(a);
    return a
}
function CWPM_OfficeApp(a) {
    return "undefined" != typeof (a) && "object" == typeof (a.Application) && ("unknown" == typeof (a.Close) || "boolean" == typeof (a.Close))
}
function CWPM_Check_Request(a) {
    var b = false;
    if (null == a) {
        b = false
    } else {
        if ("string" == typeof (a.responseText)) {
            b = true
        }
    }
    return b
}
function CWPM_Check_XmlHttp(c) {
    if (null == c) {
        return false
    }
    var a = true;
    if ("unknown" == typeof (c.status) || "number" == typeof (c.status)) {
        a = true
    } else {
        a = false
    }
    var b = navigator.userAgent;
    if (b.match(/Safari/)) {
        return "function" == typeof (c.open) && "function" == typeof (c.send) && "number" == typeof (c.readyState)
    } else {
        return a && ("unknown" == typeof (c.responseXML) || "object" == typeof (c.responseXML)) && "number" == (typeof (c.readyState))
    }
}
function CWPM_Is_Blank_Win(a) {
    var b = true;
    if ("undefined" == typeof (a)) {
        b = true
    }
    if ("_media" == a) {
        b = true
    }
    if ("_search" == a) {
        b = false
    }
    if (CWPM_Is_Win_Opened(a)) {
        b = false
    }
    if (CWPM_Is_Exist_Frame(a, top.frames)) {
        b = false
    }
    return b
}
function CWPM_Is_Exist_Frame(a, b) {
    var c = b.length;
    var d;
    for (d = 0; d < c; d++) {
        if ("undefined" == typeof (b[d])) {
            break
        }
        if (b[d].name == a) {
            return true
        }
        if (b[d].frames.length > 0) {
            if (CWPM_Is_Exist_Frame(a, b[d].frames)) {
                return true
            }
        }
    }
    return false
}
function CWPM_Is_Win_Opened(a) {
    if ("undefined" == typeof (a)) {
        return false
    }
    if ("_blank" == a) {
        return false
    }
    if ("_self" == a || "_top" == a || "_parent" == a) {
        return true
    }
    if ("undefined" != typeof (top) && CWPM_Check_Win(top) && "undefined" != typeof (top.window) && CWPM_Check_Win(top.window) && "undefined" != typeof (top.window.name) && top.window.name == a) {
        return true
    }
    var c = CWPM_Get_Opened_Wins();
    for (var b = 0; b < c.length; b++) {
        if (!c[b].closed) {
            if (c[b].name == a) {
                return true
            }
        }
    }
    return false
}
var temArray = new Array();
function CWPM_Get_Opened_Wins() {
    var a = null;
    var b;
    if (CWPM_Check_Win(top) && CWPM_Check_Win(top.window)) {
        b = top.window
    } else {
        if (CWPM_Check_Win(window)) {
            b = window
        } else {
            if (CWPM_Check_Win(parent)) {
                b = parent
            } else {
                return temArray
            }
        }
    }
    for (; typeof (b.opener) != "unknown" && !CWPM_Is_Null_Object(b.opener) && "unknown" != typeof (b.opener.closed) && !b.opener.closed && "unknown" != typeof (b.opener.top) && CWPM_Check_Win(b.opener.top); b = b.opener.top) {
        if (b == a) {
            break
        }
        a = b
    }
    if ("undefined" != typeof (b.temArray)) {
        return b.temArray
    }
    return temArray
}
function CWPM_Check_Win(a) {
    return null != a && ("unknown" != typeof (a.document) && "undefined" != typeof (a.document)) && "undefined" != typeof (a.frames) && "object" != typeof (a.outerHTML)
}
function CWPM_Check_StyleSheet(a) {
    return null != a && ("text/css" == a.type || "string" == typeof (a.cssText)) && "object" == typeof (a.parentStyleSheet)
}
function CWPM_Store_Opened_Win(d) {
    var f = CWPM_Get_Opened_Wins();
    var a = -1;
    for (var b = 0; b < f.length; b++) {
        try {
            if (f[b] == d) {
                return
            }
            if ("undefined" != typeof (f[b]) && f[b].closed) {
                a = b
            }
        } catch (c) {
            a = b
        }
    }
    if (-1 != a) {
        f[a] = d
    } else {
        if (20 > f.length) {
            f[f.length] = d
        } else {
            f[f.length - 1] = d
        }
    }
}
function CWPM_Check_Mime(a) {
    return null != a && "undefined" != typeof (a.tagName) && "mimeedit" == a.tagName.toLowerCase()
}
function CWPM_XmlParser(a) {
    return null != a && "object" == typeof (a) && "string" == typeof (a.id) && "unknown" == typeof (a.loadXML)
}
function CWPM_Xml_Doc(a) {
    return null != a && "string" == typeof (a.XML) && "object" == typeof (a.parentNode) && "string" == typeof (a.nodeValue)
}
function CWPM_Check_Xml(a) {
    if ("object" == typeof (navigator) && "undefined" != typeof (navigator.userAgent)) {
        if (null == navigator.userAgent.match("MSIE")) {
            return null != a && ("object" == typeof (a.responseXML) && "function" == typeof (a.open) && "function" == typeof (a.send))
        } else {
            return a != null && ("boolean" == typeof (a.parsed) || "unknown" == typeof (a.responseXML) || "object" == typeof (a.responseXML))
        }
    } else {
        return null != a && "object" == typeof (a.responseXML) && "function" == typeof (a.open) && "function" == typeof (a.send)
    }
}
function CWPM_Get_Location() {
    if (CWPM_Check_Location(location)) {
        return location
    } else {
        return window.document.location
    }
}
function CWPM_Get_Location(b) {
    if ("undefined" == typeof (b)) {
        if ("object" == typeof (window) && "object" == typeof (window.document) && "object" == typeof (window.document.location)) {
            var a = window.document.location.href;
            if ("javascript:" == a.match(/^javascript:/i) && "object" == typeof (top) && "object" == typeof (top.window)) {
                return top.window.document.location
            }
            return window.document.location
        }
        return 0
    } else {
        if ("string" == typeof (b)) {
            return b
        }
        if (CWPM_Check_Win(b) || CWPM_Check_Doc(b)) {
            g_CwpmLocObj = b.location;
            g_CwpmLocStr = CWPM_Origin_Url(g_CwpmLocObj.href);
            if ("" == g_CwpmLocStr) {
                return "about:blank"
            }
            return g_CwpmLocStr
        }
        if (CWPM_Check_Location(b)) {
            g_CwpmLocObj = b;
            g_CwpmLocStr = CWPM_Origin_Url(b);
            return g_CwpmLocStr
        }
        if ("undefined" != b.location) {
            return b.location
        } else {
            return b
        }
    }
}
function CWPM_Get_Hostname() {
    var a = -1;
    var b = "";
    if ("object" == typeof (window.document) && "object" == typeof (document.DSHost)) {
        b = document.DSHost;
        a = b.indexOf(":");
        if (-1 != a) {
            return b.substring(0, a)
        }
        return b
    }
    var c = CWPM_Get_Location();
    if (c) {
        return c.hostname
    }
    return ""
}
function CWPM_Check_Str(f) {
    if (0 == f.length) {
        return f
    }
    var b = 0;
    var e = f.length;
    var d = "\\0";
    var c = f.charAt(0);
    var a = (" " == c || "\t" == c || "\r" == c | "\n" == c);
    while (a && b < f.length) {
        b++;
        c = f.charAt(b)
    }
    if ('\\"' == c || "\\'" == c) {
        d = c;
        b++
    }
    c = f.charAt(e - 1);
    while (a && 0 < e) {
        e--;
        c = f.charAt(e - 1)
    }
    if (c === d) {
        e--
    }
    return f.substring(b, e)
}
function CWPM_Add_Absolute_Url(d, c) {
    if (d.match(/^http[s]?/i)) {
        return d
    }
    var e;
    var b = "";
    e = c.match(/^https?:..[^\/]+/i);
    if (null != e) {
        b = e[0]
    } else {
        return d
    }
    if ("/" == d.charAt(0)) {
        return b + d
    }
    e = c.match(/[^\?#]*\//);
    if (!e) {
        return d
    }
    var a = e[0];
    if ("." == d) {
        return a
    }
    while (d.match(/^\.\./)) {
        d = d.replace(/^\.\.\/?/, "");
        if (a != b + "/") {
            a = a.replace(/\/([^\/]+\/)$/, "/")
        }
    }
    return a + d
}
function CWPM_OriginUrl_FromPtpUrl(b) {
    var d = "";
    var c = b.toString().match(/https:\/\/([a-zA-Z0-9._-]*:?\d*)(\/.*)/);
    if ("undefined" == typeof (DSPtpHostStr)) {
        return null
    }
    var a = DSPtpHostStr.split("::");
    if (!a || (a && a.length != 2)) {
        return null
    }
    if (c && a[0] == c[1]) {
        if (c[2]) {
            d += a[1] + c[2]
        } else {
            d += a[1]
        }
        return d
    }
    return null
}
function CWPM_CodeBase_Replace(c, a) {
    c = CWPM_Check_Str(c);
    c = CWPM_Add_Absolute_Url(c, CWPM_Origin_Url(window.location.href));
    var f = CWPM_CwpmInfo_Before(c, 0, 0, 1);
    var g = f.indexOf(",CwpmInfo");
    if (-1 < g) {
        var b = f.indexOf("+", g);
        var e = "";
        if (CWPM_Need_Embed_DSID(navigator)) {
            e = "," + document.cookie.match(/DSID=[A-Za-z0-9]+/)
        }
        var d = ",CT=java";
        if (a) {
            d = ""
        }
        f = f.substring(0, g) + e + f.substring(g, b) + d + f.substring(b)
    }
    return f
}
function CWPM_Need_Embed_DSID(b) {
    if (null != b && "object" == typeof (b) && "undefined" != typeof (b.userAgent)) {
        var a = b.userAgent;
        if (a.match(/Safari/)) {
            return true
        }
        if (a.match(/Mozilla\/5\.0/)) {
            return true
        }
        if (a.match(/Java\/1.4/) && a.match(/Mozilla/)) {
            return true
        }
    }
    return false
}
function CWPM_API_Clear(c) {
    var a = CWPM_Get_Arguments_Array("clear", arguments, true);
    try {
        return CWPM_Call_Method(a)
    } catch (b) {
        return c.clear()
    }
}
function CWPM_API_Close(b) {
    var a = CWPM_Get_Arguments_Array("close", arguments, true);
    if (CWPM_Check_Doc(b)) {
        if ("unknown" != typeof (b.CwpmPrelude) && b.CwpmPrelude) {
            b.CwpmPrelude = 0
        }
        if ("unknown" != typeof (b.WriteCwpmPrelude) && b.WriteCwpmPrelude) {
            b.WriteCwpmPrelude = 0
        }
    }
    if (CWPM_Check_Win(b) && "undefined" != typeof (b.top) && "unknown" != typeof (b.top) && null != b.top && "undefined" != typeof (b.top.DSFrameToolBar) && null != b.top.DSFrameToolBar && "undefined" != typeof (b.name) && "CwpmContent" == b.name) {
        a[1] = b.top
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_Close() {
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Write(g, h) {
    var a = CWPM_Get_Arguments_Array("write", arguments, true);
    if (CWPM_Check_Doc(g)) {
        if (2 > arguments.length) {
            return CWPM_Call_Method(a)
        }
        for (var c = 2; c < arguments.length; c++) {
            h += arguments[c]
        }
        if ("boolean" == typeof (g.expando)) {
            g.expando = true
        }
        var b = "";
        if ("undefined" != typeof (g.CwpmBaseUrl) && "unknown" != typeof (g.CwpmBaseUrl)) {
            b = g.CwpmBaseUrl
        }
        CWPM_Cache_CwpmInfo(b);
        if (!CWPM_Popup_Doc(g)) {
            g.WriteCwpmPrelude = 1
        }
        if ("string" == typeof (g.SVNSaved) && g.SVNSaved.length) {
            g_SVNSaved = g.SVNSaved
        }
        g.write(CWPM_Write(g, h, 1));
        g.WriteCwpmPrelude = 0;
        g.SVNSaved = g_SVNSaved;
        g_SVNSaved = "";
        g_CwpmInfoCached = false;
        return 0
    }
    if ("undefined" == typeof (h)) {
        var f = "function anonymous(){CWPM_API_Writeln(\\\"writeln\\\",document,this.line);this.line='';}";
        var e = CWPM_Signature(g[a[0]] + "");
        var d;
        if (e == f) {
            g_CwpmTrivialWrite = 1;
            d = g[a[0]]();
            g_CwpmTrivialWrite = 0
        } else {
            d = g[a[0]]()
        }
        return d
    } else {
        return CWPM_Call_Method(a)
    }
}
function CWPM_API_Func_Write(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Write("write", arguments[1], 1)
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Writeln(e, f) {
    var a = CWPM_Get_Arguments_Array("writeln", arguments, true);
    if (CWPM_Is_Null_Object(f)) {
        return CWPM_Call_Method(a)
    }
    if (CWPM_Check_Doc(e)) {
        for (var d = 2; d < arguments.length; d++) {
            f += arguments[d]
        }
        if ("boolean" == typeof (e.expando)) {
            e.expando = true
        }
        var c = "";
        var g;
        if ("undefined" != typeof (e.CwpmBaseUrl) && "unknown" != typeof (e.CwpmBaseUrl)) {
            c = e.CwpmBaseUrl
        }
        g_CwpmInfoCached = false;
        CWPM_Cache_CwpmInfo(c);
        if (!CWPM_Popup_Doc(e)) {
            e.WriteCwpmPrelude = 1
        }
        if ("string" == typeof (e.SVNSaved) && e.SVNSaved.length) {
            g_SVNSaved = e.SVNSaved
        }
        f += "\n";
        var b = CWPM_Write(e, f, 1, g, 0);
        if ("" == g_SVNSaved || 0 < b.length) {
            e.writeln(b)
        }
        e.WriteCwpmPrelude = 0;
        e.SVNSaved = g_SVNSaved;
        g_SVNSaved = "";
        g_CwpmInfoCached = false;
        return 0
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_Writeln(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Write("writeln", arguments[1], 1, "", 1)
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Save(c, b) {
    var a = CWPM_Get_Arguments_Array("save", arguments, true);
    if (CWPM_Check_Xml(c)) {
        if ("string" == typeof (b)) {
            a[2] = CWPM_Url(b)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Submit(c) {
    var a = CWPM_Get_Arguments_Array("submit", arguments, true);
    if (CWPM_FormTag(c) && (typeof (c.getAttributeNode) == "object" || typeof (c.getAttributeNode) == "function") && typeof (c.getAttributeNode("action").value) == "string") {
        var b = c.getAttributeNode("action").value;
        if ("" != b && !b.match(/\/\d-\d+/)) {
            c.getAttributeNode("action").value = CWPM_Url(b)
        }
    } else {
        if (CWPM_FormTag(c) && typeof (c.action) == "string") {
            var b = c.action;
            if ("" != b && !b.match(/\/\d-\d+/)) {
                c.action = CWPM_Url(b)
            }
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_AddParam(d, e, b) {
    var a = CWPM_Get_Arguments_Array("addParam", arguments, true);
    var c = "";
    if ("string" == typeof (d.classid)) {
        c = d.classid.toUpperCase()
    }
    if ("string" == typeof (c) && "CLSID:A02922FD-67D8-4973-ABC6-A39E8F31ED6B" == c && ("HTTPServer" == e || "DataPath" == e || "RedirectURL" == e)) {
        a[3] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_ToString(d) {
    var a = CWPM_Get_Arguments_Array("toString", arguments, true);
    var b = 0;
    if ("object" == typeof (d) && "unknown" == typeof (d.toString)) {
        var c = "";
        if ("string" == typeof (d.name) && "string" == typeof (d.value)) {
            c = d.name
        }
        if ("To" == c || "Cs" == c || "Bcc" == c) {
            b = 1
        }
    }
    if (0 == b) {
        if (CWPM_Html_LinkTag(d)) {
            return CWPM_Origin_Url(d.href)
        }
        if (CWPM_Check_Location(d)) {
            return CWPM_Origin_Url(d.toString())
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_SetTimeout(g, c, f, h) {
    var a = [];
    if ("string" == typeof (g) && g == "VB") {
        a = new Array("setTimeout", "vbscript:" + c);
        args[args.length] = setTimeout;
        args[args.length] = "vbscript:" + c;
        for (var b = 1; b < arguments.length; b++) {
            a[a.length] = arguments[b]
        }
        if (2 == arguments.length) {
            a[a.length] = 0
        }
        return CWPM_Call_Func(a)
    }
    a[0] = "setTimeout";
    a[1] = g;
    if (CWPM_Check_Win(g)) {
        if ("string" == typeof (c)) {
            a[2] = CWPM_Js(c)
        } else {
            a[2] = c
        }
        if ("undefined" != typeof (f)) {
            a[3] = f
        }
        if ("string" == typeof (h) && "vbscript" == h.toLowerCase()) {
            a[4] = "VBScript"
        }
        if (arguments.length > 3 && a[4] != "VBScript") {
            for (var b = 4; b < arguments.length + 1; b++) {
                a[b] = arguments[b - 1]
            }
        }
    } else {
        if (arguments.length > 2) {
            if ("string" == typeof (arguments[1])) {
                a[2] = CWPM_Js(arguments[1]);
                for (var b = 3; b < arguments.length + 1; b++) {
                    a[b] = arguments[b - 1]
                }
            } else {
                for (var b = 2; b < arguments.length + 1; b++) {
                    a[b] = arguments[b - 1]
                }
            }
        }
    }
    try {
        return CWPM_Call_Method(a)
    } catch (d) {
        a = CWPM_Get_Arguments_Array("setTimeout", arguments, true);
        return CWPM_Call_Method(a)
    }
}
function CWPM_API_Func_SetTimeout(a, b) {
    if (CWPM_API_Native(arguments[0]) || 2 == arguments.length) {
        var c = CWPM_Add_Timeout_Watcher(arguments[2], CWPM_Js(arguments[1]));
        if (3 >= arguments.length) {
            return setTimeout(c, arguments[2])
        }
        arguments[1] = c
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_AddBehavior(c, b) {
    var a = CWPM_Get_Arguments_Array("addBehavior", arguments, true);
    if (2 == arguments.length && "string" == typeof (arguments[1])) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Set_Behavior(b, a) {
    if (CWPM_Check_Style(b)) {
        if (a && a.match(/\(#/i)) {
            b.behavior = a
        } else {
            a = CWPM_Url(a);
            b.behavior = a
        }
    }
    return a
}
function CWPM_API_Func_AddBehavior(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Func_InsertAdjacentHTML(a, b) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[2] == CWPM_Write(0, arguments[2], 0)
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_InsertAdjacentHTML(c, b, d) {
    var a = CWPM_Get_Arguments_Array("insertAdjacentHTML", arguments, true);
    if (3 == arguments.length) {
        a[3] = CWPM_Write(c, d, 0)
    }
    return CWPM_Call_Method(a)
}
function CWPM_Check_Div(a) {
    return null != a && "string" == typeof (a.tagName) && "div" == a.tagName.toLowerCase()
}
function CWPM_API_AppendChild(j, f) {
    if (2 == arguments.length && (CWPM_Html_Body(j) || CWPM_Check_Div(j))) {
        var b = new Array();
        b[0] = f;
        var d;
        var g;
        do {
            d = CWPM_pop(b);
            if (CWPM_Check_Frames(d) || CWPM_Html_Iframe(d)) {
                if ("undefined" != typeof (d.src)) {
                    if ("" == d.src) {
                        d.src = "0-0+"
                    } else {
                        if ("about:blank" == d.src) {
                            d.src = "about:blank"
                        }
                    }
                }
            }
            if ("undefined" != typeof (d.childNodes)) {
                for (var c = 0; c < d.childNodes.length; c++) {
                    if (CWPM_Check_Div(d.childNodes[c])) {
                        if (undefined != d.childNodes[c].innerHTML && "" != d.childNodes[c].innerHTML) {
                            g = d.childNodes[c].innerHTML;
                            CWPM_API_Set_InnerHTML(d.childNodes[c], g, false)
                        }
                    }
                    b[b.length] = d.childNodes[c]
                }
            }
        } while (b.length > 0)
    }
    try {
        var a = new Array("appendChild", j, f);
        return CWPM_Call_Method(a)
    } catch (h) {
        return arguments[0].appendChild(arguments[1])
    }
}
function CWPM_API_ExecCommand(h, g) {
    var k = CWPM_Get_Arguments_Array("execCommand", arguments, true);
    if ("ClearAuthenticationCache" == g) {
        var b = document.cookie;
        var f = "DSID=";
        var n = b.indexOf("; " + f);
        var c = 1;
        var m;
        if (-1 == n) {
            n = b.indexOf(f);
            if (-1 == n) {
                c = 0
            }
        } else {
            n += 2
        }
        if (c) {
            var j = document.cookie.indexOf(";", n);
            if (-1 == j) {
                j = b.length
            }
            m = b.substring(n + f.length, j)
        }
        var b = document.cookie;
        var f = "DSIVS=";
        var n = b.indexOf("; " + f);
        var a = 1;
        var d;
        if (n == -1) {
            n = b.indexOf(f);
            if (-1 == n) {
                a = 0
            }
        } else {
            n += 2
        }
        if (a) {
            var j = document.cookie.indexOf(";", n);
            if (-1 == j) {
                j = b.length
            }
            d = b.substring(n + f.length, j)
        }
        b = document.cookie;
        f = "DSSignInUrl=";
        n = b.indexOf("; " + f);
        var p = 1;
        var l;
        if (-1 == n) {
            n = b.indexOf(f);
            if (n == -1) {
                p = 0
            }
        } else {
            n += 2
        }
        if (p) {
            var j = document.cookie.indexOf(";", n);
            if (-1 == j) {
                j = b.length
            }
            l = b.substring(n + f.length, j)
        }
    }
    var o = CWPM_Call_Method(k);
    if ("ClearAuthenticationCache" == g) {
        var e = "Thursday, 1 Jan 2020 00:00:00 GMT;";
        if (c) {
            if ("undefined" != typeof (DSPersistentCookies) && DSPersistentCookies) {
                document.cookie = "DSID=" + m + "; path=/; expires=" + e + " secure"
            } else {
                document.cookie = "DSID=" + m + "; "
            }
        }
        if (a) {
            if ("undefined" != typeof (DSPersistentCookies) && DSPersistentCookies) {
                document.cookie = "DSIVS=" + d + ";path=/; expires=" + e + " secure"
            } else {
                document.cookie = "DSIVS=" + d + "; "
            }
        }
        if (p) {
            if ("undefined" != typeof (DSPersistentCookies) && DSPersistentCookies) {
                document.cookie = "DSSignInUrl=" + l + "; path=/; expires=" + e + " secure"
            } else {
                document.cookie = "DSSignInUrl=" + l + "; "
            }
        }
    }
    return o
}
function CWPM_API_SetInterval(d, c, b) {
    var a;
    if ("string" == typeof (d) && "VB" == d) {
        a = new Array("setInterval", "vbscript:" + c, c, 0);
        return CWPM_Call_Method(a)
    }
    a = CWPM_Get_Arguments_Array("setInterval", arguments, true);
    if (CWPM_Check_Win(d)) {
        a[2] = CWPM_Add_Timeout_Watcher(b, CWPM_Js(c))
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_SetInterval(b, a) {
    if (CWPM_Check_Func(arguments[1])) {
        arguments[1] = CWPM_Add_Timeout_Watcher(arguments[2], CWPM_Js(arguments[1]))
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_CreateObject(h, b, d) {
    var a = CWPM_Get_Arguments_Array("createObject", arguments, true);
    if (3 == arguments.length) {
        var f = "";
        var c = h.getAttribute("classid");
        if ("string" == typeof (c)) {
            f = c.toUpperCase()
        }
        if ("string" == typeof (f) && "CLSID:BD96C556-65A3-11D0-983A-00C04FC29E36" == f) {
            var g = CWPM_Url(d);
            var e = d.match(/^(https?:\/\/)?([^\/:]+(:\d+)?)/);
            if (e) {
                g = CWPM_Origin_Url(d)
            }
            if ("" != g) {
                b += g;
                a[3] = "https://" + CWPM_Get_Hostname()
            }
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_EditDocument(c, b) {
    var a = CWPM_Get_Arguments_Array("editDocument", arguments, true);
    if (2 == arguments.length || 3 == arguments.length) {
        if (("function" == typeof (c.CreateNewDocument) || "unknown" == typeof (c.CreateNewDocument)) && "function" == (typeof (c.EditDocument) || "unknown" == typeof (c.EditDocument))) {
            a[1] = CWPM_Magic_CwpmInfo(b, true)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_GetAttribute(e, c) {
    var b = CWPM_Get_Arguments_Array("getAttribute", arguments, true);
    var a = CWPM_Call_Method(b);
    if (null == a && "object" == typeof (a)) {
        return a
    }
    if ("string" == typeof (c)) {
        c = c.toLowerCase()
    }
    switch (c) {
        case "href":
        case "action":
            return CWPM_Origin_Url(a);
            break;
        case "src":
            var d = CWPM_Origin_Url(a);
            if (-1 != d.indexOf("/inc/js/jquery/jquery.min.js")) {
                return "/inc/js/jquery/jquery.min.js"
            } else {
                if (-1 != d.indexOf("/inc/js/jquery/jquery-ui.custom.min.js")) {
                    return "/inc/js/jquery/jquery-ui.custom.min.js"
                } else {
                    if (-1 != d.indexOf("/inc/js/jquery/jquery.cookie.js")) {
                        return "/inc/js/jquery/jquery.cookie.js"
                    } else {
                        if (-1 != d.indexOf("/inc/js/jquery/jquery.dynatree.min.js")) {
                            return "/inc/js/jquery/jquery.dynatree.min.js"
                        } else {
                            if (d == "http://support.huaweisymantec.com/eWebEditor/uploadfile/20120301164513001.png") {
                                return "/eWebEditor/uploadfile/20120301164513001.png"
                            } else {
                                return d
                            }
                        }
                    }
                }
            }
            break;
        case "background":
        case "backgroundImage":
        case "background-image":
            return CWPM_CSS_Origin_Url(a);
            break
    }
    return a
}
function CWPM_API_Func_GetAttribute(b) {
    var a = CWPM_Call_Func(arguments);
    switch (arguments[1]) {
        case "href":
            return CWPM_Origin_Url(a);
            break
    }
    return a
}
function CWPM_API_SetAttribute(d, b, c) {
    var a = CWPM_Get_Arguments_Array("setAttribute", arguments, true);
    switch (b) {
        case "location":
            if ("undefined" != typeof (d.location)) {
                return CWPM_API_Set_Location(d, c, 0)
            }
            break;
        case "href":
            if ("undefined" != typeof (d.href)) {
                return CWPM_API_Set_Href(d, c, 0)
            }
            break;
        case "cookie":
            if ("undefined" != typeof (d.cookie)) {
                return CWPM_API_Set_Cookie(d, c, 0)
            }
            break;
        case "domain":
            if ("undefined" != typeof (d.domain)) {
                return CWPM_API_Set_Domain(d, c, 0)
            }
            break;
        case "action":
            if ("undefined" != typeof (d.action)) {
                if (CWPM_FormTag(d) && "undefined" != typeof (d.action.tagName) && d.action.tagName.toLowerCase() == "input") {
                    a[3] = CWPM_Url(c);
                    break
                }
                return CWPM_API_Set_Action(d, c, 0)
            }
            break;
        case "src":
            if ("undefined" != typeof (d.src)) {
                return CWPM_API_Set_Src(d, c, 0)
            }
            break;
        case "background":
            if ("undefined" != typeof (d.background)) {
                return CWPM_API_Set_Background(d, c, 0)
            }
            break;
        case "backgroundImage":
            if ("undefined" != typeof (d.backgroundImage)) {
                return CWPM_API_Set_BackgroundImage(d, c, 0)
            }
            break;
        case "innerHTML":
            if ("undefined" != typeof (d.innerHTML)) {
                return CWPM_API_Set_InnerHTML(d, c, 0)
            }
            break;
        case "outerHTML":
            if ("undefined" != typeof (d.outerHTML)) {
                return CWPM_API_Set_OuterHTML(d, c, 0)
            }
            break;
        default:
            break
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_RemoveAttribute(c, b) {
    var a = CWPM_Get_Arguments_Array("removeAttribute", arguments, true);
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_SetAttribute(a, b) {
    return CWPM_Call_Func(arguments)
}
function CWPM_API_ViewDocument(c, b) {
    var a = CWPM_Get_Arguments_Array("viewDocument", arguments, true);
    if (2 == arguments.length || 3 == arguments.length) {
        if ("unknown" == typeof (c.ViewDocument) || "function" == typeof (c.ViewDocument) && ("unknown" == typeof (c.CreateNewDocument) || "function" == typeof (c.CreateNewDocument))) {
            a[2] = CWPM_CwpmInfo_Before(b, 1, 1)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_CreateElement(c, b) {
    var a = CWPM_Get_Arguments_Array("createElement", arguments, true);
    if (2 == arguments.length && CWPM_Check_Doc(c) && "string" == typeof (b) && b) {
        if (b.match(/</)) {
            a[2] = CWPM_Write(0, b, 0)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_EditDocument2(d, c, b) {
    var a = CWPM_Get_Arguments_Array("viewDocument2", arguments, true);
    if ("unknown" == typeof (d.ViewDocument) || "function" == typeof (d.ViewDocument) && ("unknown" == typeof (d.CreateNewDocument) || "function" == typeof (d.CreateNewDocument)) && "unknown" == typeof (d.EditDocument) || "function" == (typeof (d.EditDocument)) && "unknown" == typeof (d.ViewDocument2) || "function" == (typeof (d.ViewDocument2))) {
        a[2] = CWPM_Magic_CwpmInfo(c, true, false, false, b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_StartDownload(d, b, c) {
    var a = CWPM_Get_Arguments_Array("startDownload", arguments, true);
    if (3 == arguments.length && CWPM_DownloadBehavior(d)) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_SubmitChanges(b) {
    var a = CWPM_Get_Arguments_Array("submitChanges", arguments, true);
    if (CWPM_Check_ClasId(b)) {
        return CWPM_Handle_Refresh("submitChanges", b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_ViewDocument2(c, b) {
    var a = CWPM_Get_Arguments_Array("viewDocument2", arguments, true);
    if ("unknown" == typeof (c.ViewDocument) || "function" == typeof (c.ViewDocument) && ("unknown" == typeof (c.CreateNewDocument) || "function" == typeof (c.CreateNewDocument)) && "unknown" == typeof (c.EditDocument) || "function" == (typeof (c.EditDocument)) && "unknown" == typeof (c.ViewDocument2) || "function" == (typeof (c.ViewDocument2))) {
        a[2] = CWPM_Magic_CwpmInfo(b, true)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_ShowModalDialog(c, b, e, d) {
    var a = CWPM_Get_Arguments_Array("showModalDialog", arguments, true);
    if ("string" == typeof (c) && "VB" == c) {
        a = new Array("showModalDialog", b, e, d);
        return CWPM_Call_Func(a)
    }
    if ("object" == typeof (b) && "string" == typeof (b.url)) {
        b.url = CWPM_Url(b.url)
    }
    if ("string" == typeof (b)) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_ShowModalDialog(a, c, b) {
    arguments[1] = CWPM_Url(arguments[1]);
    return CWPM_Call_Func(arguments)
}
function CWPM_API_ShowModelessDialog(d, c, b) {
    var a = CWPM_Get_Arguments_Array("showModelessDialog", arguments, true);
    a[2] = CWPM_Url(c);
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_Replace(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Func_Assign(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Func_CreateStyleSheet(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Func_Navigate(a) {
    if (CWPM_API_Native(arguments[0])) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Navigate(c, b) {
    var a = CWPM_Get_Arguments_Array("navigate", arguments, true);
    if ((CWPM_Check_Win(c) && CWPM_Check_Func(c.navigate)) || (CWPM_Html_Body(c))) {
        a[2] = CWPM_Url(b)
    } else {
        if ("undefined" != typeof (c.tagName) && "DIV" == c.tagName) {
            a[2] = CWPM_Magic_CwpmInfo(b, true, true)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_ShowModelessDialog(a) {
    arguments[1] = CWPM_Url(arguments[1]);
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Func_ShowHelp(b, a) {
    if (CWPM_API_Native(arguments[0])) {
        if (1 < arguments.length) {
            if ("" == arguments[1]) {
                if (CWPM_Is_Blank_Win(arguments[2])) {
                    b = "about:blank"
                }
            } else {
                arguments[1] = CWPM_Url(arguments[1])
            }
        }
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_ShowHelp(e, d, c) {
    var b = CWPM_Get_Arguments_Array("showHelp", arguments, true);
    if (CWPM_Check_Win(e)) {
        if ("" == d) {
            if (CWPM_Is_Blank_Win(c)) {
                d = "about:blank"
            }
        } else {
            d = CWPM_Url(d)
        }
        b[2] = d;
        var a = CWPM_Call_Method(b);
        CWPM_Store_Opened_Win(b);
        return b
    }
    return CWPM_Call_Method(b)
}
function CWPM_API_Func_PostURL(a) {
    if ("undefined" == typeof (getURL) && "undefined" == typeof (postURL) && "undefined" == typeof (parseXML)) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_Func_GetURL(b, a) {
    if ("undefined" == typeof (getURL) && "undefined" == typeof (postURL) && "undefined" == typeof (parseXML)) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_ExecScript(c, b, d) {
    var a = CWPM_Get_Arguments_Array("execScript", arguments, true);
    if (b) {
        a[2] = CWPM_Js(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_CreateStyleSheet(c, b) {
    var a = CWPM_Get_Arguments_Array("createStyleSheet", arguments, true);
    if ("string" == typeof (b) && "" != b) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_SaveToURL(c, b) {
    var a = CWPM_Get_Arguments_Array("SaveToURL", arguments, true);
    if ("string" == typeof (b) && "" != b) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_OpenFromURL(c, b) {
    var a = CWPM_Get_Arguments_Array("OpenFromURL", arguments, true);
    if ("string" == typeof (b) && "" != b) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_CreateNewDocument(d, b, c) {
    var a = new Array("createNewDocument", d, b, c);
    if ("function" == (typeof (d.CreateNewDocument) || "unknown" == typeof (d.CreateNewDocument)) && "function" == (typeof (d.EditDocument) || "unknown" == typeof (d.EditDocument))) {
        if ("string" == typeof (url1)) {
            a[1] = CWPM_CwpmInfo_Before(b, 1, 1)
        }
        a[2] = CWPM_CwpmInfo_Before(c, 1, 1)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_GetResponseHeader(d, c) {
    var b = CWPM_Get_Arguments_Array("getResponseHeader", arguments, true);
    var a = CWPM_Call_Method(b);
    if (CWPM_Check_Xml(d) && "string" == typeof (c) && c.match(/location/i)) {
        a = CWPM_Origin_Url(a)
    }
    return a
}
function CWPM_API_CreateNewDocument2(e, b, d) {
    var a = CWPM_Get_Arguments_Array("createNewDocument2", arguments, true);
    if (("unknown" == typeof (e.ViewDocument) || "function" == typeof (e.ViewDocument)) && "unknown" == typeof (e.CreateNewDocument) || "function" == (typeof (e.CreateNewDocument)) && "unknown" == typeof (e.EditDocument) || ("function" == typeof (e.EditDocument)) && ("unknown" == typeof (e.ViewDocument2) || "function" == typeof (e.ViewDocument2))) {
        if ("string" == typeof (b)) {
            a[2] = CWPM_Magic_CwpmInfo(b, true)
        }
        var c = CWPM_Url_JS(d);
        a[3] = c.substring(0, c.lastIndexOf("/"))
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_InsertAdjacentElement(d, b, c) {
    var a = CWPM_Get_Arguments_Array("insertAdjacentElement", arguments, true);
    if (CWPM_Html_Iframe(c)) {
        if ("" == c.src) {
            CWPM_API_Set_Src(c, "")
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_CreateContextualFragment(c, b) {
    var a = CWPM_Get_Arguments_Array("createContextualFragment", arguments, true);
    if ("function" == typeof (c.setStart) && "string" == typeof (b)) {
        a[2] = CWPM_Write(0, b, 0)
    }
    return CWPM_Call_Method(a)
}
function CWPM_DownloadBehavior(a) {
    return (null != a && null != a.style && ("url(#default#download)" == a.style.behavior || "url('#default#download')" == a.style.behavior))
}
function CWPM_Add_Timeout_Watcher(c, e) {
    var d = "";
    var b = false;
    if (("undefined" != typeof (DanaTimeoutCtx) && 0 < DanaTimeoutCtx) || 60000 < c) {
        b = true
    }
    if (b) {
        var a = e;
        if ("function" == typeof (e)) {
            a = CWPM_Stub_Func_Create(e, c);
            return a
        }
        d = "DanaTimeoutCtx=" + c + 1 + ";" + a + ";DanaTimeoutCtx=0;"
    } else {
        d = e
    }
    return d
}
function CWPM_Stub_Func_Create(b, a) {
    if ("function" == typeof (b)) {
        var c = function() {
            g_CwpmTimeoutCtx = a + 1;
            b();
            g_CwpmTimeoutCtx = 0
        };
        return c
    }
    return null
}
function CWPM_Exec_FuncName(b) {
    if (null == b) {
        return "anonymous"
    }
    var a = b.toString().match(/function (\w*)/)[1];
    if (null == a || 0 == a.length) {
        return "anonymous"
    }
    return a
}
function CWPM_pop(b) {
    if (0 >= b.length) {
        return null
    }
    var a = b[b.length - 1];
    b.length--;
    return a
}
function CWPM_Check_DHTMLMode() {
    if ("undefined" != typeof (DanaDHTMLMode) && DanaDHTMLMode) {
        return DanaDHTMLMode
    }
    return 0
}
function CWPM_Check_Is_RewritingJs(a) {
    if (30 > a.length && 0 > !a.match(/\Wwindow\W/) && a.indexOf("location") && 0 > a.indexOf("document") && 0 > a.indexOf("open") && 0 > a.indexOf("innerHTML") && 0 > a.indexOf("src") && 0 > a.indexOf("insertAdjacentHTML") && 0 > a.indexOf("outerHTML") && 0 > a.indexOf("eval")) {
        return true
    }
    if (a.match(/Dana/)) {
        return true
    }
    return false
}
function CWPM_JsParser(b, a) {
    this.scanner = new CWPM_JsTokenScanner(b);
    this.munger = new CWPM_JsMunger(b);
    this.munger.PFo = a;
    this.IJQ = 0;
    this.wys = 0;
    this.CeL = 0;
    this.dWZ = 0
}
function CWPM_JsMunger(a) {
    this.gVw = a;
    this.CsV = new Array();
    this.ltm = 0;
    this.vxj = 0;
    this.tyC = new Array();
    this.uZC = 0;
    this.JhB = new Array();
    this.dWZ = 0;
    this.mzy = 0;
    this.lQJ = '"';
    this.LNq = new Array();
    this.cOe = new Array()
}
function CWPM_JsTokenScanner(a) {
    this.iQo = new String(a);
    this.bjm = a.length;
    this.bOr = 0;
    this.Oxu = -1;
    this.LVV = -1;
    this.NKJ = -1;
    this.lnR = 0;
    this.yBR = new CWPM_JsTokenObject();
    this.savedToken = 0
}
function CWPM_JsTokenObject() {
    this.gpV = 0;
    this.GTj = 0;
    this.dRD = 0;
    this.WYt = 0
}
function CWPM_API_Get_Src(a) {
    if (CWPM_Html_Img(a) || CWPM_Check_Elem(a) || CWPM_Check_Win(a) || CWPM_Alpha_Load_Filter(a) || CWPM_Html_Script(a)) {
        return CWPM_Origin_Url(a.src)
    }
    return a.src
}
function CWPM_API_Get_SRC(a) {
    var b;
    if ("string" == typeof (a.classid)) {
        b = a.classid.toUpperCase()
    }
    if ("CLSID:601B418B-E6A6-47FC-A094-07248741CEB3" == b && "string" == typeof (b)) {
        return CWPM_Origin_Url(a.SRC)
    }
    return a.SRC
}
function CWPM_API_Get_URL(a) {
    if (CWPM_Check_Doc(a)) {
        return CWPM_Origin_Url(a.URL)
    }
    return a.URL
}
function CWPM_API_Get_Hash(a) {
    if ("" != g_CwpmLocStr && a == g_CwpmLocStr) {
        return g_CwpmLocObj.hash
    }
    return a.hash
}
function CWPM_API_Get_Host(b) {
    if (CWPM_Check_Location(b) || CWPM_AreaTag(b) || CWPM_AnchorTag(b)) {
        var a = CWPM_Origin_Url(b.href).match(/^(https?:\/\/)?([^\/:]+(:\d+)?)/);
        if (!a) {
            return ""
        }
        return a[2]
    } else {
        if ("string" == typeof (b)) {
            var a = b.match(/^(https?:\/\/)?([^\/:]+)/);
            if (!a) {
                return ""
            }
            return a[2]
        }
    }
    return b.host
}
function CWPM_API_Set_Host(a, b) {
    a.host = b;
    return b
}
function CWPM_API_Get_Href(b) {
    if (CWPM_Check_Location(b)) {
        var a = CWPM_Origin_Url(b.href);
        if ("" == a) {
            return "about:blank"
        } else {
            return a
        }
    } else {
        if ("string" == typeof (b)) {
            return b
        } else {
            if ("string" == typeof (b.href) && b.href.match(/webproxy/)) {
                return CWPM_Origin_Url(b.href)
            }
        }
    }
    return b.href
}
function CWPM_API_Get_Port(c) {
    var b = /^(https?:\/\/)?([^\/:]+(:\d+)?)/;
    var a;
    if (CWPM_Check_Location(c)) {
        a = CWPM_Origin_Url(c.href).match(b);
        if (!a || "undefined" == typeof (a[3]) || "" == a[3]) {
            return ""
        }
        return a[3].substring(1)
    } else {
        if ("string" == typeof (c)) {
            a = c.match(b);
            if (!a || "undefined" == typeof (a[3]) || "" == a[3]) {
                return ""
            }
            return a[3].substring(1)
        }
    }
    return c.port
}
function CWPM_API_Get_Action(a) {
    if (CWPM_FormTag(a)) {
        return CWPM_Origin_Url(a.action)
    }
    if ("undefined" != typeof (a.action) && null != a.action && "string" != typeof (a.action)) {
        return a.action
    }
    return a.action
}
function CWPM_API_Get_Cookie(a) {
    if (CWPM_Check_Doc(a)) {
        return CWPM_Check_Cookie()
    }
    return a.cookie
}
function CWPM_Check_Cookie() {
    var f = "";
    var a = document.cookie.split(/;\s*/);
    var d = 0;
    var h = new Object();
    function l(m, n) {
        m = m.replace(/^\s*/, "");
        h[m] = n
    }
    var c = g_CwpmCookie.split(";");
    for (d = 0; d < c.length; d++) {
        var k = c[d].match(/(([^=]*)=)(.*)/);
        if (!k) {
            continue
        }
        l(k[2], k[3])
    }
    if ((0 <= g_CwpmCookie.indexOf("currentSelections")) && (top.DanaSplHostname == CWPM_API_Get_Domain(document))) {
        var c = top.DanaSplCookie.split(";");
        for (d = 0; d < c.length; d++) {
            var k = c[d].match(/(([^=]*)=)(.*)/);
            if (!k) {
                continue
            }
            l(k[2], k[3])
        }
    }
    for (d = 0; d < a.length; d++) {
        if ("userTem=" == a[d].substr(0, 7)) {
            continue
        }
        if (-1 != a[d].indexOf("byd.com.cn--#-SVN_WebProxy-JSESSIONID")) {
            return a[d]
        }
        var j = a[d].split("=");
        if (j.length >= 2) {
            l(j[0], a[d].substring(j[0].length + 1))
        }
        if (j.length == 1) {
            l("", j[0])
        }
    }
    var b;
    for (b in h) {
        if (f.length > 0) {
            f += "; "
        }
        var g = h[b];
        if (typeof (g) != "string") {
            continue
        }
        var e = g.indexOf(";");
        if (0 != e) {
            if (0 <= e) {
                g = g.substring(0, e)
            }
            f += b + "=" + g
        } else {
            f += b
        }
    }
    return f
}
function CWPM_API_Get_Domain(a) {
    if (CWPM_Check_Doc(a) && "undefined" != typeof (a.danaDocDomain)) {
        return a.danaDocDomain
    } else {
        if (CWPM_Doc_Url(a)) {
            return CWPM_Origin_Host(a)
        } else {
            if ("undefined" != typeof (a.domain)) {
                return a.domain
            }
        }
    }
}
function CWPM_API_Get_Filter(a) {
    if (CWPM_Check_Style(a)) {
        return CWPM_Encode_Filter(a.filter, "decode")
    }
    return a.filter
}
function CWPM_Check_Style(a) {
    return null != a && "undefined" != typeof (a.fontStyle)
}
function CWPM_API_Get_Length(b) {
    var a = 0;
    if (CWPM_Is_Null_Object(b)) {
        return 0
    }
    if (CWPM_Check_Iframe_Usage() && CWPM_Check_Frames(b)) {
        a = b.length;
        if ("undefined" != typeof (b.dstbIframe)) {
            a--
        }
        if ("undefined" != typeof (b.DSFrameToolBar)) {
            a--
        }
        if ("undefined" != typeof (b.DanaContent)) {
            a--
        }
        return a
    }
    return b.length
}
function CWPM_API_Get_Search(c) {
    var a;
    var b = /\?([^\\#]*)/;
    if (CWPM_Check_Location(c)) {
        a = CWPM_Origin_Url(c.href).match(b);
        if (!a) {
            return ""
        }
        return a[0]
    } else {
        if ("string" == typeof (c)) {
            a = c.match(b);
            if (!a) {
                return ""
            }
            return a[0]
        }
    }
    return c.search
}
function CWPM_API_Set_Search(e, g) {
    var c;
    var d = /\?([^\\#]*)/;
    var f = arguments[arguments.length - 1];
    if (3 == arguments.length && (arguments[1] instanceof Function)) {
        if (f) {
            return e.search += g
        } else {
            return e.search = g
        }
    }
    if (CWPM_Check_Location(e)) {
        if (f) {
            return e.search += g
        } else {
            return e.search = g
        }
    } else {
        if ("string" == typeof (e)) {
            if (g_CwpmLocStr == e) {
                c = e.match(d);
                if (!c) {
                    return ""
                }
                if (f) {
                    return g_CwpmLocObj.search += g
                } else {
                    return g_CwpmLocObj.search = g
                }
            } else {
                var a = new Array("search", e, g);
                return CWPM_Call_Method(a)
            }
        }
    }
    try {
        var a = CWPM_Get_Arguments_Array("search", arguments, false);
        return CWPM_Call_Method(a)
    } catch (b) {
        if (f) {
            return e.search += g
        } else {
            return e.search = g
        }
    }
}
function CWPM_API_CreatePopup(d) {
    var a = CWPM_Get_Arguments_Array("createPopup", arguments, true);
    if (1 == arguments.length) {
        if (CWPM_Check_Win(d)) {
            var b = d.createPopup();
            if (b && ("undefined" != typeof (b.document))) {
                var c = b.document;
                if (typeof (c.expando) == "boolean") {
                    c.expando = true
                }
                c.DSPopupDoc = true
            }
            return b
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Get_Behavior(c) {
    if (CWPM_Check_Style(c)) {
        var d = c.behavior + "";
        if (0 > d.indexOf("url(")) {
            return c.behavior
        } else {
            var a = d.match(/url\(\'([^\']*?)\'/);
            if (a && a[1]) {
                var b = CWPM_Origin_Url(a[1]);
                d = d.replace(/url\(\'([^\']*?)\'/, "url('" + b + "'")
            }
            return d
        }
    }
    return c.behavior
}
function CWPM_API_Get_CssText(a) {
    if (CWPM_Check_Style(a)) {
        return CWPM_CSS_Origin_Url(a.cssText)
    }
    return a.cssText
}
function CWPM_API_Get_CodeBase(a) {
    if ("string" != typeof (a)) {
        a = a.codeBase
    }
    var b = a.match(/([^?#]*\/)/);
    if (b) {
        return b[0]
    }
    return a
}
function CWPM_API_Get_Hostname(c) {
    if ("string" == typeof (c)) {
        var a = c.match(/^(https?:\/\/)?([^\/:]+)/);
        if (!a) {
            return ""
        }
        return a[2]
    } else {
        if (CWPM_Check_Location(c)) {
            var b = CWPM_API_Get_Host(c);
            var d = b.match(/[^:]+/);
            if (d) {
                return d[0]
            }
            return b
        }
    }
    return c.hostname
}
function CWPM_API_Get_Location(b) {
    if ("string" == typeof (b)) {
        return b
    }
    if (CWPM_Check_Win(b) || CWPM_Check_Doc(b)) {
        g_CwpmLocObj = b.location;
        g_CwpmLocStr = CWPM_Origin_Url(b.location.href);
        if ("" == g_CwpmLocStr) {
            return "about:blank"
        }
        return g_CwpmLocObj
    }
    try {
        return b.location()
    } catch (a) {
        return b.location
    }
}
function CWPM_API_Get_Pathname(d, c) {
    var b = /^(https?:\/\/[^\/]+)?([^?]*)/;
    if (CWPM_Check_Location(d)) {
        var a = CWPM_Origin_Url(d.href).match(b);
        if (!a) {
            return ""
        }
        return a[2]
    } else {
        if ("string" == typeof (d)) {
            var a = d.match(b);
            if (!a) {
                return ""
            }
            return a[2]
        } else {
            if ("string" == typeof (c) && "undefined" == typeof (d.pathname)) {
                return c
            }
        }
    }
    return d.pathname
}
function CWPM_API_Get_Protocol(c) {
    var b = /^([a-zA-Z]+:)/;
    var a;
    if (g_CwpmFakeProtocol) {
        g_CwpmFakeProtocol = 0;
        return "https:"
    }
    if (CWPM_Check_Location(c)) {
        a = CWPM_Origin_Url(c.href).match(b);
        if (!a) {
            return ""
        }
        return a[1]
    } else {
        if ("string" == typeof (c)) {
            a = CWPM_Origin_Url(c).match(b);
            if (!a) {
                return ""
            }
            return a[1]
        }
    }
    return c.protocol
}
function CWPM_API_Get_Referrer(a) {
    if (CWPM_Check_Doc(a)) {
        return CWPM_Origin_Url(a.referrer)
    }
    return a.referrer
}
function CWPM_API_Get_InnerHTML(b) {
    var a = b.innerHTML;
    if (a instanceof Object) {
        return b.innerHTML
    }
    if (a && a.match(/id=dstb-id/)) {
        return a.replace(/<SCRIPT\s+language="javascript"\s+id=dstb-id[^<]*<\/SCRIPT>/i, "")
    }
    return a
}
function CWPM_API_Get_Background(a) {
    if ("string" == typeof (a.background) && "undefined" != a.background) {
        return CWPM_Origin_Url(a.background)
    }
    return a.background
}
function CWPM_API_Get_DocumentHTML(d) {
    var b = d.DocumentHTML;
    var e;
    if ("string" == typeof (d.classid)) {
        e = d.classid.toUpperCase()
    }
    if (typeof (b) == "string" && ("CLSID:2D360201-FFF5-11D1-8D03-00A0C959BC0A" == e) || ("string" == typeof (d.outerHTML) && null != d.outerHTML && d.outerHTML.match(/<editing:pdec.*class=MSGBODY[^\/]*<\/editing:pdec>$/))) {
        var c = CWPM_Unrewrite_Post_Body(b);
        var a = new RegExp("<SCRIPT[s]+.*dsbaseurl[^>]*>g_baseurl.*</SCRIPT[s]*[^>]*>", "gi");
        c = c.replace(a, "");
        return c
    } else {
        return b
    }
}
function CWPM_API_Get_ChildNodes(g) {
    if (CWPM_Html_Head(g) || CWPM_Html_Body(g)) {
        var b = new Array();
        var f = g.childNodes;
        var d = 0;
        var c = false;
        for (var e = 0; e < f.length; e++) {
            var a = false;
            var h = f[e];
            if ("undefined" != typeof (h.tagName)) {
                if ("script" == h.tagName.toLowerCase()) {
                    if ((h.src.indexOf("/dana-cached/js/") != -1) || (h.innerHTML.indexOf("var dsnodocwrites") != -1) || (h.innerHTML.indexOf("var DanaCode") != -1) || (h.innerHTML.indexOf("if(dschk()") != -1) || (h.innerHTML.indexOf("dsSetTimers(") != -1) || (h.innerHTML.indexOf("var DSTBSettings") != -1) || (h.innerHTML.indexOf("dstb") != -1) || (h.innerHTML.indexOf("CwpmFirstScript") != -1)) {
                        a = true;
                        c = true
                    }
                } else {
                    if ("!" == h.tagName) {
                        if (c) {
                            a = true
                        }
                    } else {
                        if ("" == h.innerHTML && "/noscript" == h.tagName.toLowerCase()) {
                            a = true;
                            c = true
                        } else {
                            if ("iframe" == h.tagName.toLowerCase() && h.src.match(/cwpm-cached\/help/)) {
                                a = true;
                                c = true
                            } else {
                                if ("div" == h.tagName.toLowerCase() && h.innerHTML.match(/Reposition toolbar/)) {
                                    a = true;
                                    c = true
                                }
                            }
                        }
                    }
                }
            }
            if (!a) {
                b[d] = h;
                d++
            }
        }
        if (c) {
            return b
        }
    }
    return g.childNodes
}
function CWPM_API_Get_FirstChild(a) {
    if (CWPM_Html_Body(a)) {
        var b = CWPM_API_Get_ChildNodes(a);
        if (null == b || 0 == b.length) {
            return null
        }
        return b[0]
    }
    return a.firstChild
}
function CWPM_API_Set_Src(d, b, a) {
    if ("string" == typeof (b) && "" == b && CWPM_Check_Xml(d)) {
        if (a) {
            d.src += b
        } else {
            d.src = b
        }
        return b
    }
    var c = true;
    if (CWPM_Html_Iframe(d) && CWPM_IsMacSafari() && b.match(/about:blank/)) {
        c = false
    }
    if ("string" == typeof (b) && b.match(/about:blank/)) {
        return b
    }
    if ((CWPM_Html_Img(d) || CWPM_Check_Elem(d) || CWPM_Check_Win(d) || CWPM_Alpha_Load_Filter(d)) && c) {
        if (CWPM_Html_Script(d)) {
            if (a) {
                d.src = CWPM_Url(CWPM_Origin_Url(d.src) + b, 3)
            } else {
                if ("" == b) {
                    d.src = b
                } else {
                    d.src = CWPM_Url(b, 3)
                }
            }
        } else {
            if (a) {
                if (CWPM_Html_Frame(d) || CWPM_Html_Iframe(d)) {
                    d.src = CWPM_Url(CWPM_Origin_Url(d.src) + b, true)
                } else {
                    d.src = CWPM_Url(CWPM_Origin_Url(d.src) + b)
                }
            } else {
                if (CWPM_Html_Frame(d) || CWPM_Html_Iframe(d)) {
                    d.src = CWPM_Url(b, true)
                } else {
                    d.src = CWPM_Url(b)
                }
            }
        }
    } else {
        if (a) {
            d.src += b
        } else {
            d.src = b
        }
    }
    return b
}
function CWPM_API_Set_SRC(c, b, a) {
    var d;
    if ("string" == typeof (c.classid)) {
        d = c.classid.toUpperCase()
    }
    if ("string" == typeof (d) && "CLSID:601B418B-E6A6-47FC-A094-07248741CEB3" == d) {
        if (a) {
            c.SRC = CWPM_Url(CWPM_Origin_Url(c.SRC) + b)
        } else {
            c.SRC = CWPM_Url(b)
        }
    } else {
        if (a) {
            c.SRC += b
        } else {
            c.SRC = b
        }
    }
    return b
}
function CWPM_API_Set_URL(d, c, b) {
    var e;
    if (typeof (c) == "object") {
        var a = CWPM_Get_Arguments_Array("URL", arguments, false);
        return CWPM_Call_Method(a)
    }
    if (typeof (d.classid) == "string") {
        e = d.classid.toUpperCase()
    }
    if (CWPM_Check_Win(d) || CWPM_Check_Doc(d) || CWPM_Is_Web_ReportSource(d) || "string" == (typeof (e) && ("CLSID:79348936-6D8C-11D2-8B4E-00AA0030C99A" == e || "CLSID:BD96C556-65A3-11D0-983A-00C04FC29E33" == e))) {
        if (b) {
            d.URL = CWPM_Url(CWPM_Origin_Url(d.URL) + c)
        } else {
            d.URL = CWPM_Url(c)
        }
    } else {
        if (b) {
            d.URL += c
        } else {
            d.URL = c
        }
    }
    return c
}
function CWPM_API_Set_Href(d, b, a) {
    if ("string" == typeof (d) && d == g_CwpmLocStr) {
        if (a) {
            g_CwpmLocObj.href = CWPM_Url(CWPM_Origin_Url(g_CwpmLocObj.href)) + b
        } else {
            g_CwpmLocObj.href = CWPM_Url(b)
        }
        return b
    }
    if (CWPM_Html_LinkTag(d) || CWPM_AnchorTag(d) || CWPM_AreaTag(d) || CWPM_Check_Location(d) || CWPM_Check_Elem(d) || CWPM_Check_Win(d) || CWPM_Check_StyleSheet(d)) {
        var c;
        if (CWPM_Check_Location(d)) {
            if (!(d.href + "").match(/,CwpmInfo=/) || CWPM_Origin_Url(d.href) != b) {
                c = CWPM_Url(b);
                d.href = c;
                return b
            }
        }
        if (a) {
            c = CWPM_Url(CWPM_Origin_Url(d.href) + b)
        } else {
            c = CWPM_Url(b)
        }
        d.href = c
    } else {
        if (a) {
            d.href += b
        } else {
            d.href = b
        }
    }
    return b
}
function CWPM_Check_Func(b) {
    if (b) {
        if ((b instanceof Function) && (null == navigator.userAgent.match("MSIE"))) {
            return true
        }
        var a = b + "";
        if (a) {
            if (null != navigator && "object" == typeof (navigator) && "undefined" != typeof (navigator.userAgent)) {
                if (a.match(/^\s*function[^{]*\{\s*\[\s*native\s*code\s*\]\s*\}\s*$/)) {
                    return true
                }
                if (navigator.userAgent.match(/Safari/)) {
                    if (a.match(/^\s*\[function\]\s*$/)) {
                        return true
                    }
                }
                return false
            } else {
                return false
            }
        } else {
            if ("object" == typeof (b) && "undefined" == typeof (b.constructor)) {
                return true
            }
        }
    }
    return false
}
function CWPM_Magic_CwpmInfo(b, f, g, e, d) {
    var c = b;
    var a = CWPM_CwpmInfo_Before(b, true, true);
    var h = a.indexOf(",DSID=");
    var j = a.indexOf("+", h);
    c = a.substring(0, h) + a.substring(j + 1);
    CWPM_Reset_CwpmInfo(a, f, 0, c, g, e, d);
    return c
}
function CWPM_CwpmInfo_Before(c, k, h, f) {
    var b = "";
    c = CWPM_Url(c);
    var e = c.indexOf(",CwpmInfo");
    if (-1 >= e) {
        return c
    }
    var n = false;
    if (0 < e && "/" == c.charAt(e - 1)) {
        n = true
    }
    var p = c.indexOf("+", e);
    var o = p + 1;
    if (n && c.charAt(p + 1) == "/") {
        o++
    }
    b = c.substring(0, e);
    var l = b.lastIndexOf("/");
    if (l >= 0) {
    }
    if (k) {
        var a = document.cookie;
        var d = "DSID=";
        var j = a.indexOf("; " + d);
        var n = true;
        var m;
        if (-1 == j) {
            j = a.indexOf(d);
            if (-1 == j) {
                n = false
            }
        } else {
            j += 2
        }
        if (n) {
            var g = document.cookie.indexOf(";", j);
            if (-1 == g) {
                g = a.length
            }
            m = a.substring(j + d.length, g)
        }
        b += ",DSID=" + m
    }
    b += c.substring(e, p);
    if (h) {
        b += ",CT=MSP"
    }
    if (f) {
        b += ",ST=1+/"
    } else {
        b += "+"
    }
    b += c.substring(o);
    return b
}
function CWPM_Signature(a) {
    a = a.replace(/\\r\\n/g, "");
    a = a.replace(/\s/g, "");
    a = a.replace(/\t/g, "");
    return a
}
function CWPM_Reset_CwpmInfo(f, q, g, h, a, o, n) {
    if (q && !g) {
        g = 60000
    }
    var r = document.cookie;
    var t = "DSID=";
    var e = r.indexOf("; " + t);
    var c = 1;
    var j;
    if (-1 == e) {
        e = r.indexOf(t);
        if (-1 == e) {
            c = 0
        }
    } else {
        e += 2
    }
    if (c) {
        var d = document.cookie.indexOf(";", e);
        if (-1 == d) {
            d = r.length
        }
        j = r.substring(e + t.length, d)
    }
    t = "DSIVS=";
    e = r.indexOf("; " + t);
    var l = 1;
    if (-1 == e) {
        e = r.indexOf(t);
        if (-1 == e) {
            l = 0
        }
    } else {
        e += 2
    }
    if (l) {
        d = document.cookie.indexOf(";", e);
        if (-1 == d) {
            d = r.length
        }
        j = r.substring(e + t.length, d)
    }
    t = "DSSignInUrl=";
    e = r.indexOf("; " + t);
    var u = 1;
    if (-1 == e) {
        e = r.indexOf(t);
        if (-1 == e) {
            u = 0
        }
    } else {
        e += 2
    }
    if (u) {
        d = document.cookie.indexOf(";", e);
        if (-1 == d) {
            d = r.length
        }
        j = r.substring(e + t.length, d)
    }
    var b = CWPM_Url(f);
    document.cookie = CWPM_Set_Cook("CwpmInfo=" + b, true, g + 7200000);
    var g = (new Date((new Date()).getTime() + g)).toUTCString();
    var k = h.replace(/https:\/+[^\/]*/i, "");
    var m = k.substring(k.lastIndexOf("/") + 1);
    if (/\.[a-zA-Z0-9_]+$/.test(m)) {
        k = k.substring(0, k.lastIndexOf("/") + 1)
    }
    if (a) {
        k = encodeURI(k)
    }
    g = (new Date((new Date()).getTime() + g + 7200000)).toUTCString();
    if ("/" != k) {
        document.cookie = "DSExternalReq=1;path=" + k + ";secure; expires=" + g;
        if (o) {
            document.cookie = "DSExternalReq=1; path=" + k.toLowerCase() + "; secure; expires=" + g
        }
    }
    document.cookie = "DSExternalReq=1; path=/_vti_bin/;  secure; expires=" + g;
    if (n && "SharePoint.WebPartPage.document" == n) {
        var p = h.match(/https:\/+[^\/]*(\/[^\/]*\/)/);
        if (p && p[1]) {
            if ("/" != p[1]) {
                document.cookie = "DSExternalReq=1;path=" + p[1] + ";secure;  expires=" + g
            }
            document.cookie = "DSExternalReq=1; path=/_layouts/images/; secure; expires=" + g
        }
    }
    if (c) {
        if (typeof (DSPersistentCookies) != "undefined" && DSPersistentCookies) {
            var g = "Thursday, 1 Jan 2030 00:00:00 GMT;";
            document.cookie = "DSID=" + j + "; path=/; expires=" + g + " secure"
        } else {
            document.cookie = "DSID=" + j + "; "
        }
    }
    if (l) {
        if (typeof (DSPersistentCookies) != "undefined" && DSPersistentCookies) {
            var g = "Thursday, 1 Jan 2030 00:00:00 GMT;";
            document.cookie = "DSIVS=" + j + ";  path=/;  expires=" + g + " secure"
        } else {
            document.cookie = "DSIVS=" + j + ";  path=/;secure"
        }
    }
    if (u) {
        if (typeof (DSPersistentCookies) != "undefined" && DSPersistentCookies) {
            var g = "Thursday, 1 Jan 2030 00:00:00 GMT;";
            document.cookie = "DSSignInUrl=" + j + "; path=/; expires=" + g + " secure"
        } else {
            document.cookie = "DSSignInUrl=" + j + "; "
        }
    }
}
function CWPM_API_Set_Port(c, b, a) {
    if ("string" == typeof (c) && c == g_CwpmLocStr && "number" == typeof (b)) {
        g_CwpmLocObj.href = CWPM_Url(CWPM_Origin_Url(g_CwpmLocObj.href), ",Port=" + b);
        return b
    }
    if ((CWPM_Check_Location(c) || CWPM_Check_Win(c) || CWPM_Check_Elem(c) || CWPM_Check_StyleSheet(c)) && "number" == typeof (b)) {
        c.href = CWPM_Url(CWPM_Origin_Url(c.href), ",Port=" + b)
    } else {
        if (a) {
            c.port += b
        } else {
            c.port = b
        }
    }
    return b
}
function CWPM_API_Set_Action(c, b, a) {
    if ("" != b && CWPM_FormTag(c)) {
        if (a) {
            c.action = CWPM_Url(CWPM_Origin_Url(c.action) + b)
        } else {
            c.action = CWPM_Url(b)
        }
    } else {
        if (a) {
            c.action += b
        } else {
            c.action = b
        }
    }
    return b
}
function CWPM_API_Set_Cookie(h, d, a) {
    if (CWPM_Check_Doc(h)) {
        var b = /\/webproxy\/\d+\/\d+\//;
        var g = [];
        var f = "";
        var c = 0;
        if (typeof (window) == "object" && (typeof (window.document) == "object" || typeof (window.location) == "object")) {
            if (typeof (window.document.URL) != "undefined") {
                f = window.document.URL
            } else {
                f = window.location + ""
            }
        } else {
            if (typeof (top) == "object" && typeof (top.document) == "object") {
                f = top.document.URL
            }
        }
        g = f.match(b);
        if (!g) {
            document.cookie = d;
            return document.cookie
        }
        var e = d.split(";");
        for (c = 0; c < e.length; c++) {
            if (e[c].indexOf("path=") >= 0) {
                d = d.replace(e[c].split("=")[1], g[0])
            } else {
                if (e[c].indexOf("domain=") >= 0) {
                    d = d.replace("oa.byd.com.cn", "byd.com.cn");
                    d = d.replace(e[c].split("=")[1], f.match(/^https?:\/\/([a-zA-Z0-9._-]*)/)[1])
                }
            }
        }
        document.cookie = d;
        return document.cookie
    }
    return h.cookie = arguments[1]
}
function CWPM_API_Set_Cursor(c, b) {
    if (b && "string" == typeof (b) && b.match(/url\(/)) {
        var a = CWPM_CSS_Url(b);
        c.cursor = a
    } else {
        c.cursor = b
    }
    return b
}
function CWPM_API_Set_Domain(b, a) {
    if (CWPM_Check_Doc(b)) {
        if ("undefined" == typeof (b.expando) || "boolean" == typeof (b.expando)) {
            b.expando = true;
            b.danaDocDomain = a
        } else {
            a = document.domain;
            b.domain = a
        }
    } else {
        b.domain = a
    }
    return a
}
function CWPM_API_Set_Filter(h, c, j, g, d) {
    if ("boolean" == typeof (j)) {
        if (CWPM_Check_Func(c)) {
            return h.filter = c
        }
        var b = c;
        if (CWPM_Check_Style(h)) {
            if ("string" != typeof (c)) {
                if (null != c && "function" == typeof (c.toString)) {
                    c = c.toString()
                } else {
                    if (j) {
                        h.filter += c
                    } else {
                        h.filter = c
                    }
                    return c
                }
            }
            b = CWPM_Encode_Filter(c, "encode")
        }
        try {
            return h.filter(arguments[1])
        } catch (f) {
            if (j) {
                h.filter += b
            } else {
                h.filter = b
            }
            return b
        }
    } else {
        var a = CWPM_Get_Arguments_Array("filter", arguments, false);
        return CWPM_Call_Method(a)
    }
}
function CWPM_API_Set_BaseUrl(d, b, a) {
    var c = "";
    if ("string" == typeof (d.classid)) {
        c = d.classid.toUpperCase()
    }
    if ("string" == typeof (c) && "CLSID:6E449683-C509-11CF-AAFA-00AA00B6015C" == c) {
        if (a) {
            d.BaseUrl = CWPM_Url(CWPM_Origin_Url(d.BaseUrl) + b)
        } else {
            d.BaseUrl = CWPM_Url(b)
        }
    } else {
        if (a) {
            d.BaseUrl += b
        } else {
            d.BaseUrl = b
        }
    }
    return b
}
function CWPM_API_Set_BaseURL(d, b, a) {
    var c = "";
    if ("string" == typeof (d.classid)) {
        c = d.classid.toUpperCase()
    }
    if ("string" == typeof (c) && b && ("CLSID:2D360201-FFF5-11D1-8D03-00A0C959BC0A" == c || "CLSID:747B28E6-FBD9-4AE9-B298-E395F00C68BA" == c || "CLSID:F32F8372-5871-4B5C-86C4-1C06E76C1147" == c)) {
        if (a) {
            d.BaseURL = CWPM_Url(CWPM_Origin_Url(d.BaseURL) + b)
        } else {
            d.BaseURL = CWPM_Url(b)
        }
    } else {
        if (a) {
            d.BaseURL += b
        } else {
            d.BaseURL = b
        }
    }
    return b
}
function CWPM_API_Set_CssText(c, b, a) {
    if (CWPM_Check_Style(c) || CWPM_Check_StyleSheet(c)) {
        if (a) {
            c.cssText = CWPM_CSS_Url(CWPM_CSS_Origin_Url(c.cssText) + b, 1)
        } else {
            c.cssText = CWPM_CSS_Url(b, 1)
        }
    } else {
        if (a) {
            c.cssText += b
        } else {
            c.cssText = b
        }
    }
    if (typeof (c.cssText) != "unknown") {
        return c.cssText
    }
}
function CWPM_API_Set_DataURL(d, b, a) {
    var c = "";
    if ("string" == typeof (d.classid)) {
        c = d.classid.toUpperCase()
    }
    if ("CLSID:333C7BC4-460F-11D0-BC04-0080C7055A83" == c && "string" == typeof (c)) {
        if (a) {
            d.DataURL = CWPM_Url(CWPM_Origin_Url(d.DataURL) + b)
        } else {
            d.DataURL = CWPM_Url(b)
        }
    } else {
        if (a) {
            d.DataURL += b
        } else {
            d.DataURL = b
        }
    }
    return b
}
function CWPM_API_Set_CodeBase(c, b, a) {
    if (null != c && "string" == typeof (c.tagName) && c.tagName.match(/^(object|applet)$/i)) {
        if (a && c.codeBase) {
            b = CWPM_Origin_Url(c.codeBase + "") + b;
            c.codeBase = CWPM_CodeBase_Replace(b)
        }
    } else {
        if (a) {
            c.codeBase += b
        } else {
            c.codeBase = b
        }
    }
    return b
}
function CWPM_API_Set_Location(d, b, a) {
    if (CWPM_Check_Win(d) || CWPM_Check_Doc(d) || CWPM_Check_Frames(d)) {
        var c = "";
        if (a) {
            c = CWPM_Url(CWPM_Origin_Url(d.location.href) + b)
        } else {
            c = CWPM_Url(b)
        }
        d.location = c
    } else {
        if (a) {
            d.location += b
        } else {
            d.location = b
        }
    }
    return b
}
function CWPM_API_Func_Set_Location(b, a) {
    var c = "";
    if (b) {
        c = CWPM_Url(b)
    }
    if ("undefined" == typeof (a)) {
        a = false
    }
    if (true == a) {
        window.location += c
    } else {
        window.location = c
    }
    return c
}
function CWPM_API_Func_Get_Location() {
    return CWPM_Origin_Url(window.location.href)
}
function CWPM_API_Set_Pathname(c, b, a) {
    if (CWPM_Check_Location(c)) {
        if (a) {
            c.pathname = CWPM_Url(CWPM_API_Get_Pathname(c) + b)
        } else {
            c.pathname = CWPM_Url(b)
        }
    } else {
        if (a) {
            c.pathname += b
        } else {
            c.pathname = b
        }
    }
    return b
}
function CWPM_API_Set_InnerHTML(g, d, a) {
    if ("undefined" == typeof (g)) {
        return d
    }
    try {
        if ("string" == typeof (g.id) && null != g.id && "spnXEnroll" == g.id) {
            if ("" == g_cwpmInfo) {
                var b = window.location.href.match(/,CwpmInfo=([a-zA-Z0-9.-]*)(,Port=(\d+))?(,SSL)?/);
                g_cwpmInfo = b[0];
                g_cwpmInfo = g_cwpmInfo.substring(g_cwpmInfo.indexOf(",DanaInfo=") + 10);
                g_cwpmInfo += "+"
            }
            document.cookie = CWPM_Set_Cook("CwpmInfo=" + g_cwpmInfo);
            g_cwpmInfo = ""
        }
        if ("undefined" != typeof (g.tagName) && "XML" == g.tagName.toUpperCase()) {
            g.innerHTML = d;
            return d
        }
        var c;
        if (g_SVNUseCache) {
            pBk = CWPM_Check_Cache("CWPM_API_Set_InnerHTML", arguments);
            if ("undefined" == typeof (pBk)) {
                dsIncrementMissCount("CWPM_API_Set_InnerHTML", arguments);
                pBk = CWPM_Write(g, d, 0);
                dsAddToCache("DanaPutInnerHTML", arguments, pBk)
            }
        } else {
            c = CWPM_Write(g, d, 0)
        }
        if (a) {
            g.innerHTML += c
        } else {
            g.innerHTML = c
        }
    } catch (f) {
        if (a) {
            g.innerHTML += d
        } else {
            g.innerHTML = d
        }
    }
    return d
}
function CWPM_API_Set_OuterHTML(f, c, b) {
    var a = "";
    try {
        if ("undefined" != typeof (f.tagName)) {
            a = CWPM_Write(f, c, 0)
        }
    } catch (d) {
        if (b) {
            f.outerHTML += c
        } else {
            f.outerHTML = c
        }
        return c
    }
    if (1 == g_SVNaxflag && CWPM_Check_IEVersion(6)) {
        if (b) {
            f.outerHTML += a
        } else {
            f.outerHTML = a
        }
    } else {
        if (b) {
            f.outerHTML += a
        } else {
            f.outerHTML = a
        }
    }
    return c
}
function CWPM_API_Set_Background(c, b, a) {
    if ("string" == typeof (c.background)) {
        if ("undefined" == typeof (c.tagName) && "undefined" != typeof (c.backgroundImage)) {
            if (typeof (b) == "string" && b.match(/[\s]url\s*\(|^url\s*\(/i)) {
                if (a) {
                    c.background += CWPM_CSS_Url(b)
                } else {
                    c.background = CWPM_CSS_Url(b)
                }
                return b
            }
        } else {
            if ("string" == typeof (c.tagName) && "undefined" != typeof (c.style)) {
                if (a) {
                    c.background = CWPM_Url(CWPM_Origin_Url(c.background) + b)
                } else {
                    c.background = CWPM_Url(b)
                }
                return b
            }
        }
    }
    if (a) {
        c.background += b
    } else {
        c.background = b
    }
    return b
}
function CWPM_API_Set_DocumentHTML(c, b, a) {
    var d = "";
    if ("string" == typeof (c.classid)) {
        d = c.classid.toUpperCase()
    }
    if ("string" == typeof (d) && "CLSID:2D360201-FFF5-11D1-8D03-00A0C959BC0A" == d) {
        if (a) {
            c.DocumentHTML += CWPM_Write(0, b, 0)
        } else {
            c.DocumentHTML = CWPM_Write(0, b, 0)
        }
    } else {
        if (a) {
            c.DocumentHTML += b
        } else {
            c.DocumentHTML = b
        }
    }
    return b
}
function CWPM_API_StringCheckLink(a) {
    if ("undefined" == typeof (a)) {
        return ""
    }
    if ("Boolean" == typeof (a)) {
        return a
    }
    if (CWPM_Html_LinkTag(a)) {
        return CWPM_Origin_Url(a.href)
    }
    if (CWPM_Check_Location(a)) {
        return CWPM_Origin_Url(a.toString())
    }
    return a
}
function CWPM_API_Set_ReportSource(d, b, a) {
    var e = "";
    if ("string" == typeof (d.classid)) {
        e = d.classid.toUpperCase()
    }
    if ("string" == typeof (e) && "CLSID:C4847596-972C-11D0-9567-00A0C9273C2A" == e || "CLSID:A1B8A30B-8AAA-4A3E-8869-CLSID:C4847596-972C-11D0-9567-00A0C9273C2A" == e || "CLSID:2DEF4530-8CE6-41C9-84B6-A54536C90213" == e) {
        var c = CWPM_Url(b);
        if (c != b) {
            b = c
        }
        if (a) {
            d.ReportSource += b
        } else {
            d.ReportSource = b
        }
    } else {
        if (a) {
            d.ReportSource += b
        } else {
            d.ReportSource = b
        }
    }
    return b
}
function CWPM_API_Set_BackgroundImage(c, d, a) {
    if (CWPM_Check_Style(c)) {
        if (a) {
            c.backgroundImage = CWPM_CSS_Url(CWPM_Origin_Url(c.backgroundImage) + d)
        } else {
            c.backgroundImage = CWPM_CSS_Url(d)
        }
    } else {
        try {
            if (a) {
                c.style.backgroundImage += CWPM_CSS_Url(d)
            } else {
                c.style.backgroundImage = CWPM_CSS_Url(d)
            }
        } catch (b) {
            c.backgroundImage = d
        }
    }
    return d
}
function CWPM_Check_Cache(c, b) {
    var a;
    var d = CWPM_Get_Key(b);
    if (null == d) {
        return OFi
    }
    a = g_SVNCache[c].ShZ[d];
    if ("undefined" != typeof (a)) {
        a = g_SVNCache[c].ShZ[d].value;
        g_SVNCache[c].ShZ[d].STT++;
        if (g_SVNCache[c].rIU) {
            g_SVNCache[c].rIU /= 2
        }
    }
    return a
}
function CWPM_Get_Key(a) {
    var c = "";
    for (var b = 0; a && b < a.length; b++) {
        if (typeof (a[b]) != "undefined") {
            c += a[b] + "_"
        }
    }
    if ("" == c) {
        return null
    } else {
        return c
    }
}
function CWPM_Is_Web_ReportSource(a) {
    return ("undefined" != typeof (a) && "string" == typeof (a.URL) && "object" == typeof (a.ReportSource) && "boolean" == typeof (a.PromptOnRefresh))
}
function CWPM_Check_ScriptTag(a) {
    return CWPM_Check_Elem(a) && "string" == typeof (a.tagName) && "SCRIPT" == a.tagName.toUpperCase() && "string" == typeof (a.nodeName) && "SCRIPT" == a.nodeName.toUpperCase()
}
function CWPM_IsMacSafari() {
    var a = navigator.userAgent;
    if (a.match(/Safari/) && a.match(/Mac OS/)) {
        return true
    }
    return false
}
function CWPM_API_Get_BackgroundImage(a) {
    if (CWPM_Check_Style(a)) {
        return CWPM_CSS_Origin_Url(a.backgroundImage)
    }
    return a.backgroundImage
}
function CWPM_Unrewrite_Post_Body(d) {
    var a = "";
    var f = "https://" + CWPM_Get_Hostname();
    var h = d.indexOf(f);
    while (-1 != h) {
        a += d.substring(0, h);
        d = d.substring(h);
        var b = d.indexOf("+");
        var k = d.toLowerCase().indexOf("%2b");
        if (b != -1 && k != -1) {
            if (k < b) {
                b = k + 2
            }
        }
        if (-1 == b) {
            b = k;
            if (-1 != b) {
                b += 2
            }
        }
        var j = false;
        if (-1 != b) {
            var g = d.substring(0, b + 1);
            var c = g.match(/https:..[a-zA-Z0-9._-]*(\/.*),DanaInfo=([a-zA-Z0-9._-]*)(,Port=(\d*))?(,SSL)?(,CT=[a-zA-Z]*)?(,SSO=[A-Z]?)?(,BAU=(.*),BAP=(.*))?(\+|%2[bB])/);
            if (c && !CWPM_IsEmptyStr(c[2])) {
                var e = "";
                if (!CWPM_IsEmptyStr(c[5])) {
                    e += "https://"
                } else {
                    e += "http://"
                }
                if (!CWPM_IsEmptyStr(c[9]) && !CWPM_IsEmptyStr(c[10])) {
                    e += c[9] + ":" + c[10] + "@"
                }
                if ("." == c[2].charAt(0)) {
                    e += CWPM_Decode_Hostname(c[2])
                } else {
                    e += c[2]
                }
                if (!CWPM_IsEmptyStr(c[4])) {
                    e += ":" + c[4]
                }
                if (!CWPM_IsEmptyStr(c[1])) {
                    e += c[1]
                }
                a += e;
                d = d.substring(g.length);
                j = true
            }
        }
        if (!j) {
            a += f;
            d = d.substring(f.length)
        }
        h = d.indexOf(f)
    }
    a += d;
    a = a.replace(/,CwpmInfo=.*\+/g, "");
    return a
}
function CWPM_Decode_Hostname(a) {
    var e = "";
    var d;
    var f = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.";
    var h = f.length;
    switch (a.charAt(1)) {
        case 97:
        case 98:
            for (var g = 2; g < a.length; g++) {
                d = a.charAt(g);
                if ("-" == d) {
                    e += "+"
                } else {
                    if ("." == d) {
                        e += "="
                    } else {
                        if ("_" == d) {
                            e += "/"
                        } else {
                            e += d
                        }
                    }
                }
            }
            return CWPM_Base64_Decode(e);
        default:
            for (var g = 2; g < a.length; g++) {
                d = a.charAt(g);
                var c = f.indexOf(d);
                if (0 > c) {
                    e += d
                } else {
                    var b = c - (g - 2);
                    if (0 > b) {
                        b += h
                    }
                    e += f.charAt(b)
                }
            }
            return e
    }
}
function CWPM_Base64_Decode(g) {
    var f = "";
    var a;
    var d;
    var c = 0;
    var e;
    var b = -1;
    for (var h = 0; h < g.length; h++) {
        d = g.charAt(h);
        if ("A" <= d && "Z" >= d) {
            a = g.charCodeAt(h) - 65
        } else {
            if ("a" <= d && "z" >= d) {
                a = g.charCodeAt(h) - 97 + 26
            } else {
                if ("0" <= d && "9" >= d <= "9") {
                    a = g.charCodeAt(h) - 48 + 52
                } else {
                    if (d == "+") {
                        a = 62
                    } else {
                        if (d == "/") {
                            a = 63
                        } else {
                            continue
                        }
                    }
                }
            }
        }
        b++;
        switch (b % 4) {
            case 0:
                c = a;
                continue;
            case 1:
                e = (c << 2) | (a >> 4);
                c = a & 15;
                break;
            case 2:
                e = (c << 4) | (a >> 2);
                c = a & 3;
                break;
            case 3:
                e = (c << 6) | (a >> 0);
                c = a & 0;
                break
        }
        if ((e < 32 || e > 126) && (e != 13) && (e != 10)) {
            f = f + "<";
            f = f + "0123456789ABCDEF".charAt((e / 16) & 15);
            f = f + "0123456789ABCDEF".charAt((e / 1) & 15);
            f = f + ">"
        } else {
            f = f + String.fromCharCode(e)
        }
    }
    return f.toString()
}
function CWPM_Base64_Encode(e) {
    var b = "";
    var f = 0;
    var d;
    var c = 0;
    var a = 0;
    for (f = 0; f < e.length; f++) {
        a = e.charCodeAt(f);
        switch (f % 3) {
            case 0:
                d = (a & 252) >> 2;
                c = a & 3;
                break;
            case 1:
                d = (c << 4) | ((a & 240) >> 4);
                c = a & 15;
                break;
            case 2:
                d = (c << 2) | ((a & 192) >> 6);
                c = (a & 63);
                break
        }
        b = b + CWPM_Base64_To_Ascii(d);
        if (2 == (f % 3)) {
            b = b + CWPM_Base64_To_Ascii(c)
        }
    }
    switch (e.length % 3) {
        case 0:
            break;
        case 1:
            c = c << 4;
            b = b + CWPM_Base64_To_Ascii(c);
            b = b + "==";
            break;
        case 2:
            c = c << 2;
            b = b + CWPM_Base64_To_Ascii(c);
            b = b + "=";
            break
    }
    return b
}
function CWPM_Base64_To_Ascii(b) {
    var a;
    if (0 <= b && 25 >= b) {
        a = String.fromCharCode(b + 65)
    } else {
        if (26 <= b && 51 >= b) {
            a = String.fromCharCode(b - 26 + 97)
        } else {
            if (52 <= b && 61 >= b) {
                a = String.fromCharCode(b - 52 + 48)
            } else {
                if (62 == b) {
                    a = "+"
                } else {
                    if (63 == b) {
                        a = "/"
                    } else {
                        a = String.fromCharCode(255)
                    }
                }
            }
        }
    }
    return a
}
function CWPM_IsEmptyStr(a) {
    return ("" == a || "string" != typeof (a)) ? true : false
}
function CWPM_CSS_Origin_Url(a) {
    if ("string" != typeof (a)) {
        return a
    }
    return CWPM_Check_CSS_Url_Common(a, "decode")
}
function CWPM_Check_CSS_Url_Common(c, d, l) {
    var j = "";
    var g;
    if ((g = c.match(/(filter\s*:\s*)([^;]*)/i))) {
        var f = c.substring(0, g.index);
        f += g[1];
        var a = g[2];
        a = CWPM_Encode_Filter(a, d);
        f += a;
        f += c.substring(g.lastIndex);
        c = f
    }
    while (1) {
        var h = 0;
        var e = c.match(/[Uu][rR][lL]\s*\(\s*((\"([^\"\)]*)\")|(\'([^\'\)]*)\')|([^\)]*))\)/);
        if (!e) {
            e = c.match(/@import\s+((\"([^\"]*)\")|(\'([^\']*)\')|([^;\s]*));/);
            if (!e) {
                break
            }
            h = 1
        }
        var m = "";
        j += c.substring(0, e.index);
        if (h) {
            j += "@import "
        } else {
            j += "url("
        }
        if (j.match(/@import\s*$/)) {
            m = ",CT=css"
        }
        var b;
        var k;
        if (e[3]) {
            k = '"';
            b = e[3]
        } else {
            if (e[5]) {
                k = "'";
                b = e[5]
            } else {
                k = "";
                b = e[6]
            }
        }
        j += k;
        if (d == "encode") {
            if (l && !b.match(/^(https?:\/\/)?([^\/:]+)/i)) {
                j += "https://" + CWPM_Get_Hostname() + CWPM_Url(b, m)
            } else {
                j += CWPM_Url(b, m)
            }
        } else {
            j += CWPM_Origin_Url(b)
        }
        j += k;
        if (h) {
            j += ";"
        } else {
            j += ")"
        }
        c = c.substring(e.index + e[0].length)
    }
    j += c;
    return j
}
function CWPM_Check_Iframe_Usage() {
    if (-1 == g_CwpmIframeUsage) {
        if ("undefined" != typeof (g_HSTBSettings)) {
            if (g_HSTBSettings & (2)) {
                g_CwpmIframeUsage = 1
            } else {
                if (g_HSTBSettings & 65536) {
                    if (CWPM_Check_IEVersion(6) || navigator.userAgent.match(/Firefox\//)) {
                        g_CwpmIframeUsage = 1
                    }
                }
            }
        }
    }
    return g_CwpmIframeUsage == 1 ? true : false
}
function CWPM_Check_IEVersion(b) {
    if (null != navigator && "object" == typeof (navigator) && "string" == typeof (navigator.userAgent)) {
        var a = navigator.userAgent.toLowerCase().match(/msie\s+(\d\.\d+)/);
        if (a && a[1]) {
            var c = parseFloat(a[1]);
            if (c >= b) {
                return true
            }
        }
    }
    return false
}
function CWPM_Check_Frames(a) {
    return null != a && "object" == typeof (a) && "object" == typeof (a.frames) && "undefined" != typeof (a.length) && "undefined" != typeof (a.frameElement)
}
function CWPM_Encode_Filter(a, f) {
    var b;
    if ("string" != typeof (a)) {
        return a
    }
    var g = a.indexOf("AlphaImageLoader");
    if (g > 0) {
        var h = a.substring(g);
        var e = null;
        var d = /src\s*=\s*((\"([^\"]*)\")|(\'([^\']*)\')|([^,\)]*))/i;
        if ((e = h.match(d))) {
            b = a.substring(0, g);
            b += h.substring(0, e.index);
            b += "src=";
            var c;
            var j;
            if ("" != e[3]) {
                j = '"';
                c = e[3]
            } else {
                if ("" != e[5]) {
                    j = "'";
                    c = e[5]
                } else {
                    j = "";
                    c = e[6]
                }
            }
            b += j;
            if ("encode" == f) {
                b += CWPM_Url(c)
            } else {
                b += CWPM_Origin_Url(c)
            }
            b += j;
            b += h.substring(e.index + e[0].length)
        } else {
            return a
        }
    } else {
        return a
    }
    return b
}
function CWPM_Origin_Host(b) {
    var a;
    if (CWPM_Check_Doc(b)) {
        a = CWPM_Origin_Url(b.location.href).match(/https?:\/\/([a-zA-Z0-9._-]*)/);
        if (!a) {
            if ("undefined" != typeof (b.parentWindow)) {
                if (typeof (b.parentWindow.top) != "undefined") {
                    a = CWPM_Origin_Url(b.parentWindow.top.document.location.href).match(/https?:\/\/([a-zA-Z0-9._-]*)/)
                }
            }
        }
        if ("undefined" != typeof (b.domain) && !a) {
            return b.domain
        }
    } else {
        a = CWPM_Origin_Url(top.document.location.href).match(/https?:\/\/([a-zA-Z0-9._-]*)/)
    }
    if (!a) {
        return b.url
    }
    return a[1]
}
function CWPM_Doc_Url(a) {
    return !CWPM_Is_Null_Object(a) && CWPM_Check_Doc_Domain(a) && "undefined" != typeof (a.URL)
}
function CWPM_AnchorTag(a) {
    return "undefined" != typeof (a.tagName) && "a" == a.tagName.toLowerCase()
}
function CWPM_AreaTag(a) {
    return "undefined" != typeof (a.tagName) && "area" == a.tagName.toLowerCase()
}
function CWPM_Alpha_Load_Filter(a) {
    return "undefined" != typeof (a.src) && "undefined" != typeof (a.daL)
}
function CWPM_Origin_Url(h) {
    if (CWPM_Is_Null_Object(h)) {
        return null
    } else {
        if ("" == h || h.match(/\/cwpm-cached\/help\/empty.html/)) {
            return ""
        } else {
            if (h.match(/\/cwpm-cached\/help\/blank.html/)) {
                return "about:blank"
            }
        }
    }
    var a = h.match(/[0-9]{1}[-][0-9]{1}[+]/);
    if (null == a) {
        return h
    }
    var o = a[0].charAt(2);
    var k = h.indexOf("/webproxy/");
    var j = h.indexOf("http/", k);
    if (-1 == j) {
        j = h.indexOf("https/", k)
    }
    var q = h.indexOf("?");
    var p = 0;
    if (-1 == q) {
        p = h.lastIndexOf("/")
    } else {
        p = h.lastIndexOf("/", q)
    }
    var l = h.indexOf(a[0]);
    if (l > 0 && h.charAt(l - 1) != "/") {
        return h
    }
    var d = h.substring(l + a[0].length);
    if (-1 == j) {
        var t = h.substring(0, p + 1);
        var n = h.match(/^([/]+)/i);
        if (n) {
            t = t.replace(/..\//g, "");
            h = t + d
        } else {
            if (h.match(/^([a-zA-Z0-9]+\/)/i)) {
                h = t + d
            } else {
                h = t + d
            }
        }
        return h
    }
    var m = h.substring(j, p + 1);
    h = m + d;
    h = h.replace("http/", "http://").replace("https/", "https://");
    if (0 < (o & 4)) {
        var r = h.toString().match(/https?:\/\/([a-zA-Z0-9._-]*:?([a-zA-Z0-9.])*)/);
        var n = h.match(/^(https?:\/\/)/i);
        if (n) {
            var e = CWPM_Arr(r[1], aucDomain);
            var b = h.indexOf(r[1]);
            var f = h.substring(0, b);
            var g = h.substring(b + r[1].length);
            CWPM_UrlDecode(e, e.length);
            var c = e.join("");
            h = f + c + g
        }
    }
    return h
}
function CWPM_API_Assign(c, b) {
    var a = CWPM_Get_Arguments_Array("assign", arguments, true);
    if (CWPM_Check_Location(c)) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Reload(c, b) {
    var a = CWPM_Get_Arguments_Array("reload", arguments, true);
    if (CWPM_Check_Location(c) && 2 == arguments.length && "string" == typeof (b)) {
        a[2] = CWPM_Url(b)
    }
    if ("string" == typeof (c)) {
        a[1] = g_CwpmLocObj;
        if (2 == arguments.length) {
            a[2] = CWPM_Url(b)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Func_Reload(a) {
    if (CWPM_API_Native(arguments[0]) && 1 < arguments.length) {
        arguments[1] = CWPM_Url(arguments[1])
    }
    return CWPM_Call_Func(arguments)
}
function CWPM_API_AddRule(d, c, b) {
    var a = CWPM_Get_Arguments_Array("addRule", arguments, true);
    if (b && "undefined" != typeof (d.cssText) && "undefined" != typeof (d.rules)) {
        a[3] = CWPM_CSS_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_CSS_Url(a, b) {
    if ("string" != typeof (a)) {
        return a
    }
    return CWPM_Check_CSS_Url_Common(a, "encode", b)
}
function CWPM_API_LoadURL(c, b) {
    var a = CWPM_Get_Arguments_Array("loadURL", arguments, true);
    if (CWPM_Check_Elem(c)) {
        a[2] = CWPM_Url(b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Initialize(e, b) {
    if (1 == arguments.length) {
        return arguments[0].initialize()
    }
    var a = CWPM_Get_Arguments_Array("initialize", arguments, true);
    var f;
    if (typeof (e.classid) == "string") {
        f = e.classid.toUpperCase()
    }
    if ((typeof (f) == "string") && (f == "CLSID:CDE67FFE-F739-11D2-812A-00C04F762641")) {
        var d = CWPM_Url(b);
        var c = b.match(/^(https?:\/\/)?([^\/:]+(:\d+)?)/);
        if (c) {
            d = CWPM_Origin_Url(d)
        }
        if (d != "") {
            a[2] = "https://" + CWPM_Get_Hostname()
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_Get_Arguments_Array(f, c, e) {
    var a = new Array();
    a[0] = f;
    if (typeof (e) == "undefined") {
        e = true
    }
    var b = c.length;
    if (!e) {
        b -= 1
    }
    for (var d = 0; d < b; d++) {
        a[d + 1] = c[d]
    }
    return a
}
function CWPM_API_Refresh(b) {
    var a = CWPM_Get_Arguments_Array("refresh", arguments, true);
    if (CWPM_Check_ClasId(b)) {
        return CWPM_Handle_Refresh(refresh, b)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Replace(c, d, b) {
    var a = CWPM_Get_Arguments_Array("replace", arguments, true);
    if ("string" == typeof (c)) {
        if (3 == arguments.length) {
            return CWPM_Call_Method(a)
        }
        if (c == g_CwpmLocStr) {
            a[1] = g_CwpmLocObj;
            a[2] = CWPM_Url(d);
            return CWPM_Call_Method(a)
        }
    }
    if (CWPM_Check_Location(c) && !CWPM_MIME_Header_Array_Value(c)) {
        a[2] = CWPM_Url(d);
        return CWPM_Call_Method(a)
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_ReplaceState(d, c, e, b) {
    var a = CWPM_Get_Arguments_Array("replaceState", arguments, true);
    if ("string" == typeof (b)) {
        if (-1 == b.indexOf("/webproxy/")) {
            a[4] = CWPM_Url(b)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_PushState(d, c, e, b) {
    var a = CWPM_Get_Arguments_Array("pushState", arguments, true);
    if ("string" == typeof (b)) {
        if (-1 == b.indexOf("/webproxy/")) {
            a[4] = CWPM_Url(b)
        }
    }
    return CWPM_Call_Method(a)
}
function CWPM_API_Set_WebUrl(c, b, a) {
    if ("string" == typeof (b)) {
        b = CWPM_Url(b);
        c.WebUrl = b;
        return b
    }
}
function CWPM_API_Get_WebUrl(a) {
    if ("string" == typeof (a.WebUrl)) {
        return CWPM_Origin_Url(a.WebUrl)
    }
}
function CWPM_MIME_Header_Array_Value(a) {
    return ((null != a) && ("unknown" == typeof (a.add)) && ("unknown" == typeof (a.remove)) && ("unknown" == typeof (a.replace)))
}
function CWPM_Handle_Refresh(d, e) {
    var a = e.Server;
    var c = CWPM_Url(a);
    var f = a.match(/^(https?:\/\/ )?([^\/:]+(:\d + )?)/);
    if (f) {
        c = c.substring(c.indexOf(",CwpmInfo = ") + 10)
    }
    if ("" != c) {
        document.cookie = CWPM_Set_Cook("CwpmInfo=" + c);
        e.Server = "https://" + CWPM_Get_Hostname()
    }
    var b = CWPM_Call_Method(arguments);
    e.Server = a;
    return b
}
function CWPM_Set_Cook(g, b, a) {
    if (g_CwpmCookie.length > 0) {
        g_CwpmCookie += "; "
    } else {
        g_CwpmCookie = ""
    }
    var c = "; ";
    var e = g.match(/^([^=;]*)=\s*(\S+.*)?/);
    if (!e) {
        g_CwpmCookie += g;
        return g + c
    }
    if ("undefined" != typeof (e[2])) {
        var f = CWPM_Remove_NonStdAttributes(g);
        g_CwpmCookie += e[1] + "=" + e[2].replace(/;.*$/, "");
        var d = f;
        if (b) {
            if (!a) {
                a = 60000
            }
            var h = (new Date((new Date()).getTime() + a)).toUTCString();
            return d + "; expires=" + h
        }
        return d
    } else {
        g_CwpmCookie += e[1] + "=;";
        return e[1] + "=" + c
    }
}
function CWPM_Remove_NonStdAttributes(c) {
    var f = c.split(";");
    if (f.length <= 1) {
        document.cookie = f[0];
        return f[0]
    }
    document.cookie = f[0];
    var b = f[0];
    var a = {secure: 1,discard: 1,httponly: 1};
    var h = {domain: 1,comment: 1,commenturl: 1,path: 1,expires: 1,port: 1,version: 1,"max-age": 1};
    for (var e = 1; e < f.length; e++) {
        var d = f[e].match(/(([^=]*)=)(.*)/);
        if (f[e] != "") {
            document.cookie = f[e];
            var g = "";
            if (!d) {
                g = f[e].replace(/(\s*)(\S*)(\s*)/, "$2")
            } else {
                g = d[2].replace(/(\s*)(\S*)(\s*)/, "$2");
                a = h
            }
            if ("undefined" != typeof (a[g.toLowerCase()])) {
                b += "; " + f[e]
            }
        }
    }
    return b
}
function CWPM_Check_ClasId(a) {
    var b;
    if ("string" == typeof (a.classid)) {
        b = a.classid.toUpperCase()
    }
    if ("string" == typeof (b) && "CLSID:BD96C556-65A3-11D0-983A-00C04FC29E33" == b) {
        return true
    }
    return false
}
function CWPM_Check_Elem(a) {
    return (a != null && "string" == typeof (a.outerHTML) && "string" == typeof (a.className)) || CWPM_Html_Input(a) || CWPM_Html_Frame(a) || CWPM_Html_Script(a)
}
function CWPM_Html_Input(a) {
    return null != a && "string" == typeof (a.tagName) && "input" == a.tagName.toLowerCase()
}
function CWPM_Html_Frame(a) {
    return null != a && "string" == typeof (a.tagName) && "frame" == a.tagName.toLowerCase()
}
function CWPM_Html_Head(a) {
    return null != a && "string" == typeof (a.tagName) && "head" == a.tagName.toLowerCase()
}
function CWPM_Html_Img(a) {
    return null != a && "string" == typeof (a.tagName) && "img" == a.tagName.toLowerCase()
}
function CWPM_Html_Head(a) {
    return null != a && "string" == typeof (a.tagName) && "head" == a.tagName.toLowerCase()
}
function CWPM_HtmlTag(a) {
    return null != a && "string" == typeof (a.tagName) && "html" == a.tagName.toLowerCase()
}
function CWPM_Html_Iframe(a) {
    return null != a && "string" == typeof (a.tagName) && "iframe" == a.tagName.toLowerCase()
}
function CWPM_Html_Script(a) {
    return null != a && "string" == typeof (a.tagName) && "script" == a.tagName.toLowerCase()
}
function CWPM_Html_Body(a) {
    return null != a && "string" == typeof (a.tagName) && "body" == a.tagName.toLowerCase()
}
function CWPM_Html_LinkTag(a) {
    return null != a && "string" == typeof (a.tagName) && "link" == a.tagName.toLowerCase()
}
function CWPM_FormTag(a) {
    return null != a && "string" == typeof (a.tagName) && "form" == a.tagName.toLowerCase()
}
function CWPM_ParaseUrl(b) {
    var g = b.indexOf("?");
    var c = "";
    if (-1 < g) {
        c = b.substring(g);
        b = b.substring(0, g)
    }
    b = b.replace(/([^/])(\.\.\/)/g, "$1/$2");
    if (null == b.match(/\/\.\.\//)) {
        return b + c
    }
    var f = false;
    var h = "";
    var d = b.split("/");
    var a = new Array();
    var j = 0;
    if (null != b.match(/^http(s)?:\/\//)) {
        h = d[0] + "/" + d[1] + "/" + d[2] + "/";
        f = true;
        j = 3
    } else {
        if (null != b.match(/^\/\//)) {
            f = true;
            if (b.match(/^\/\/[a-zA-Z0-9][-a-zA-Z0-9]{0,62}(\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})+\.?/)) {
                h = d[0] + "/" + d[1] + "/" + d[2] + "/";
                s;
                j = 3
            } else {
                h = "/";
                j = 2
            }
        } else {
            if (null != b.match(/^\//)) {
                h = "/";
                j = 1;
                f = true
            }
        }
    }
    if (f) {
        for (var e = j; e < d.length; e++) {
            if (".." != d[e]) {
                a.push(d[e])
            } else {
                if (0 < a.length) {
                    a.pop()
                }
            }
        }
    } else {
        for (var e = 0; e < d.length; e++) {
            if (".." != d[e]) {
                a.push(d[e])
            } else {
                if (0 < a.length && ".." != a[a.length - 1]) {
                    a.pop()
                } else {
                    a.push(d[e])
                }
            }
        }
    }
    return h + a.join("/") + c
}
function CWPM_Url(g, C) {
    if (!g || g.match(/about:blank/i)) {
        return g
    }
    if ("string" == typeof (g) && 0 == g.indexOf("https:") && -1 != g.indexOf("webproxy") && null == g.match(/[0-9]{1}[-][0-9]{1}[+]/)) {
        var ab = g.indexOf("http/");
        var o = "";
        if (-1 != ab) {
            o = g.substring(ab + 5);
            g = o.substring(o.indexOf("/"))
        } else {
            if (-1 != (ab = g.indexOf("https/"))) {
                o = g.substring(ab + 6);
                g = o.substring(o.indexOf("/"))
            }
        }
    }
    g = CWPM_ParaseUrl(g);
    var Y = "";
    if ("boolean" == typeof (C)) {
        Y = parent.location.href
    } else {
        if (typeof (window) == "object" && (typeof (window.document) == "object" || typeof (window.location) == "object")) {
            if (typeof (window.document.URL) != "undefined") {
                Y = window.document.URL
            } else {
                Y = window.location + ""
            }
        } else {
            if (typeof (top) == "object" && typeof (top.document) == "object") {
                Y = top.document.URL
            }
        }
    }
    if ((g.indexOf("/javascript/cwpmjs.js") >= 0) || (g.indexOf("/javascript/wpmconfig.js") >= 0)) {
        return g
    }
    var H = Y.indexOf("/webproxy/");
    var f = H;
    var d = H;
    var V = 0;
    for (Z = H; Z < Y.length; Z++) {
        if (Y.charAt(Z) == "/") {
            V++;
            if (V == 5) {
                f = Z
            } else {
                if (V == 7) {
                    d = Z;
                    break
                }
            }
        }
    }
    var Q = Y.substring(0, f + 1);
    if (g.indexOf("/kns/Request/login.aspx?pt=") >= 0) {
        var P = Q + "http/epub.cnki.net/"
    } else {
        var P = Y.substring(0, d + 1)
    }
    if ("string" != typeof (g)) {
        if (("object" == typeof (g)) && (null != g)) {
            g = g.toString()
        } else {
            return g
        }
    }
    var U = 0;
    for (var Z = 0; Z < g.length; Z++) {
        if (32 >= g.charCodeAt(Z)) {
            U = Z + 1;
            continue
        }
        break
    }
    g = g.substring(U);
    var j = 0;
    var n = g.match(/(\.\.\/)/g);
    if (null != n) {
        j = n.length
    }
    var W = g;
    if ("?" == g.charAt(0) || "#" == g.charAt(0) || "javascript:void(0)" == g || "javascript:void(0);" == g) {
        return g
    }
    if (g.match(/^javascript:/)) {
        return CWPM_Js(g)
    }
    if ("http://" == g || "//:" == g) {
        return g
    }
    if ("" == g) {
        return "0-0+"
    }
    var q = g.indexOf("?");
    var ac = g.indexOf("#");
    var u = g;
    var w = "";
    if (q >= 0) {
        w = g.substring(q);
        g = g.substring(0, q)
    } else {
        if (ac >= 0) {
            w = g.substring(ac);
            g = g.substring(0, ac)
        }
    }
    var ab = g.indexOf(":");
    var X = g.indexOf("/");
    var F = g.indexOf("\\");
    if (-1 != ab && (ab < X || ab < F)) {
        var M = g.substring(0, ab + 1);
        if (!M.match(/^https?:/)) {
            return g
        }
    }
    var D = g.match(/[0-9]{1}[-][0-9]{1}[+]/);
    if (null != D) {
        return u
    }
    var m = Y.match(/\//g);
    var x = m.length;
    var R = g.toString().match(/^(https?:\/\/)?([a-zA-Z0-9._-]*:?\d*)/);
    var k = g.toString().match(/^(https?:\/\/)?([a-zA-Z0-9._-]*)/);
    var O = Y.match(/https?/g);
    var E = g.match(/^((https?:)?\/\/)/i);
    if (E && "//" == E[0]) {
        if (g.match(/^\/\/[a-zA-Z0-9][-a-zA-Z0-9]{0,62}(\.[a-zA-Z0-9][-a-zA-Z0-9]{0,62})+\.?/)) {
            g = O[1] + ":" + g;
            E = g.match(/^((https?:)?\/\/)/i);
            R = g.toString().match(/^(https?:\/\/)?([a-zA-Z0-9._-]*:?\d*)/);
            k = g.toString().match(/^(https?:\/\/)?([a-zA-Z0-9._-]*)/)
        } else {
            g = g.substring(1);
            E = g.match(/^\/\//)
        }
    }
    var G = g.lastIndexOf("/");
    var T = g.indexOf("?");
    if (-1 != T && G > g.indexOf("?")) {
        G = g.lastIndexOf("/", G)
    }
    var N = g.substring(0, G + 1);
    var ad = g.substring(G + 1);
    var L = "0";
    if (!Y) {
        return "/cwpm-cached/help/empty.html"
    }
    if ("number" == typeof (C) && 0 != C) {
        L = C + ""
    } else {
        var K = g.lastIndexOf(".");
        if (-1 != K) {
            var o = "";
            var v = g.indexOf("?", K);
            var t = g.indexOf("#", K);
            if (-1 != v && v > t) {
                o = g.substring(K + 1, v)
            } else {
                if (-1 != t && v < t) {
                    o = g.substring(K + 1, t)
                } else {
                    o = g.substring(K + 1)
                }
            }
            if ("js" == o.toLowerCase()) {
                L = "3"
            } else {
                if ("vbs" == o.toLowerCase()) {
                    L = "4"
                } else {
                    if ("css" == o.toLowerCase()) {
                        L = "5"
                    } else {
                        if ("xml" == o.toLowerCase() || "xsl" == o.toLowerCase()) {
                            L = "6"
                        } else {
                            if ("flv" == o.toLowerCase() || "swf" == o.toLowerCase()) {
                                L = "7"
                            } else {
                                if ("pdf" == o.toLowerCase()) {
                                    L = "9"
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    var p = (1 == g_CWPMURLEncode) ? "4" : "0";
    if (E) {
        if (k) {
            var y = CWPM_Compare(k[2], g_CWPMBypassURL);
            if (y) {
                return g
            }
        }
        if (1 == g_CWPMURLEncode) {
            var b = CWPM_Arr(R[2], aucDomain);
            var J = g.indexOf(R[2]);
            var c = g.substring(0, J);
            var aa = g.substring(J + R[2].length);
            CWPM_UrlEncode(b, b.length);
            var A = b.join("");
            N = c + A + aa
        }
        N = N.replace("http://", "http/").replace("https://", "https/");
        var h = g.match(/\//g);
        if (2 == h.length || (3 == h.length && g.indexOf("/") == g.length - 1)) {
            if (0 == g_CWPMURLEncode) {
                g = Q + N + ad + "/" + L + "-" + p + "+" + w
            } else {
                g = Q + N + "/" + L + "-" + p + "+" + w
            }
        } else {
            if (0 == g_CWPMURLEncode) {
                g = Q + N + L + "-" + p + "+" + ad + w
            } else {
                G = N.lastIndexOf("/");
                N = N.substring(0, G + 1);
                g = Q + N + L + "-" + p + "+" + ad + w
            }
        }
        return g
    } else {
        if (g.charAt(0) == "/") {
            D = Y.match(/[0-9]{1}[-][0-9]{1}[+]/);
            var e = D[0].charAt(2);
            if (1 == g_CWPMURLEncode && 0 == (e & 4)) {
                var B = 5;
                var S = Y.lastIndexOf("http/");
                if (-1 == S) {
                    B = 6;
                    S = Y.lastIndexOf("https/")
                }
                var l = Y.indexOf("/", S + B);
                var r = Y.substring(S + B, l);
                var b = CWPM_Arr(r, aucDomain);
                CWPM_UrlEncode(b, b.length);
                var A = b.join("");
                P = P.replace(r, A)
            }
            g = P + N.substring(1) + L + "-" + p + "+" + ad + w;
            return g
        } else {
            var a = CWPM_Origin_Url(Y);
            var z = a.match(/\//g).length;
            if (0 < j && ((z - 3) < j)) {
                N = N.replace(/^(\.\.\/)+/g, "");
                if (3 == z || 2 == z) {
                    return Y.substring(0, Y.lastIndexOf("/") + 1) + N + L + "-" + p + "+" + ad + w
                }
                var I = "";
                for (var Z = 0; Z < (z - 3); Z++) {
                    I += "../"
                }
                return g = I + N + L + "-" + p + "+" + ad + w
            }
            g = N + L + "-" + p + "+" + ad + w;
            return g
        }
    }
}
function CWPM_Tag_Write(d) {
    var o = d.toLowerCase();
    var b = ["<img ", "<a "];
    var l = [];
    var j = 0;
    while (true) {
        var h = -1;
        var m = -1;
        for (var f = 0; f < b.length; f++) {
            if (!b[f]) {
                continue
            }
            var k = o.indexOf(b[f], j);
            if (-1 == tagIndexStart) {
                b[f] = "";
                continue
            }
            if (-1 == h || tagIndexStart < h) {
                h = tagIndexStart;
                m = f
            }
        }
        if (-1 == h) {
            break
        }
        l[l.length] = d.substring(j, h);
        var k = o.indexOf(">", h);
        if (0 > k) {
            break
        }
        var a;
        var n;
        if (1 > m) {
            l[l.length] = "<img";
            a = " src=";
            n = h + 4
        } else {
            l[l.length] = "<a";
            a = " href=";
            n = h + 2
        }
        var e = o.indexOf(a, n);
        var c = -1;
        if (0 <= e && e + a.length < k - 1) {
            e += a.length;
            var g = d.substring(e, e + 1);
            if ("'" == g || '"' == g || "`" == g) {
                e++;
                c = o.indexOf(g, e + 1)
            } else {
                c = o.indexOf(" ", e);
                if (-1 == c || c > k) {
                    c = k - 1
                }
            }
        }
        if (-1 != c) {
            l[l.length] = d.substring(n, e);
            l[l.length] = CWPM_Url_JS(d.substring(e, c));
            l[l.length] = d.substring(c, k)
        } else {
            l[l.length] = d.substring(n, k)
        }
        j = k
    }
    l[l.length] = d.substring(j);
    return l.join("")
}
function CWPM_Check_Location(a) {
    if (null != a) {
        if (null != a && "undefined" != typeof (a.tagName)) {
            return false
        } else {
            if (null != a && "undefined" != typeof (a.pathname) && "undefined" != typeof (a.protocol) && "undefined" != typeof (a.href)) {
                return true
            } else {
                return false
            }
        }
    }
}
function CWPM_Popup_Doc(b) {
    if ("undefined" != b && g_jsonDoc.cwpmDoc == b) {
        if (null != g_jsonDoc.DSPopupDoc) {
            return g_jsonDoc.DSPopupDoc
        } else {
            var a = !CWPM_Is_Null_Object(b) && "undefined" != (typeof (b.DSPopupDoc)) && (b.DSPopupDoc);
            g_jsonDoc.DSPopupDoc = a;
            return a
        }
    }
}
function CWPM_Cache_CwpmInfo(a) {
    if (null != g_cwpmInfo && g_cwpmUrl == a) {
        g_CwpmInfoCached = true;
        return
    }
    g_cwpmUrl = a;
    CWPM_CwpmInfo();
    g_CwpmInfoCached = true
}
function CWPM_CwpmInfo() {
    if (g_CwpmInfoCached) {
        return
    }
    var b;
    var a = false;
    if ("" != g_cwpmUrl) {
        b = g_cwpmUrl + "";
        a = true
    } else {
        if ("undefined" != typeof (g_cwpmUrl) && "" != g_cwpmUrl) {
            b = g_cwpmUrl;
            a = true
        }
    }
    if (!a) {
        if ("object" == typeof (window) && (typeof (window.document) || "object" == typeof (window.location))) {
            if ("unknown" != typeof (window.document.URL)) {
                b = window.document.URL
            } else {
                b = window.location + ""
            }
        } else {
            if ("object" == typeof (top) && "object" == typeof (top.document)) {
                b = top.document.URL
            }
        }
    }
    if (b && b.indexOf("javascript:") == 0) {
        b = top.document.location + "";
        if (b && b.indexOf("javascript:") == 0) {
            b = "https://" + CWPM_Get_Hostname()
        }
    }
    if ("undefined" == typeof (b) || 0 == b.length) {
        if ("undefined" != typeof (g_url)) {
            b = g_url
        }
    }
    var c = b.match(/,CwpmInfo=([a-zA-Z0-9_.-]*)(,Port=(\d+))?(,SSL)?/);
    if (!c) {
        g_cwpmInfo = ""
    } else {
        g_cwpmInfo = c[0]
    }
    c = b.match(/^(https?:\/\/([^\/]*:[^\/]*@)?[a-zA-Z0-9_.-]*)/i);
    if (!c) {
        g_cwpmProtocolInfo = ""
    } else {
        g_cwpmProtocolInfo = c[1]
    }
}
function CWPM_Check_Doc(c, b) {
    if (c && !b && g_jsonDoc.cwpmDoc == c) {
        return true
    }
    var a = (!CWPM_Check_Top(c) && !CWPM_Is_Null_Object(c) && ("undefined" != typeof (c.location) || "undefined" != typeof (c.url)) && CWPM_Check_Doc_Domain(c));
    if (!b && a) {
        g_jsonDoc.cwpmDoc = c
    }
    return a
}
function CWPM_Check_Doc_Domain(b) {
    var a = false;
    if ("undefined" != typeof (b.domain)) {
        a = true
    }
    return a
}
function CWPM_Check_Top(a) {
    return "undefined" != typeof (a) && a == top
}
function CWPM_Is_Null_Object(a) {
    if (null != a && "undefined" != typeof (a)) {
        return false
    }
    return true
}
function CWPM_Check_CanSet(d, e) {
    var a = new Array("html", "head", "title", "link", "meta", "style", "script", "base");
    if ("string" != typeof (e)) {
        return
    }
    var c = "";
    if ("/" != e.charAt[1]) {
        c = e.substring(1, 7)
    } else {
        c = e.substring(2, 8)
    }
    c = c.toLowerCase();
    for (var b = 0; b < a.length; b++) {
        if (-1 != c.indexOf(a[b])) {
            return
        }
    }
    if ("boolean" == typeof (d.expando)) {
        d.expando = true;
        d.CwpmCanBaseSet = false
    } else {
        g_IsSet = false
    }
}
function CWPM_Absolute_CodeBase_Url(c, b) {
    c = CWPM_Check_Str(c);
    b = CWPM_Check_Str(b);
    if (b.match(/^http/i)) {
        return b
    }
    var a = CWPM_Origin_Url(CWPM_Get_Location().href);
    if (0 < c.length) {
        if (c.match(/^http/i)) {
            a = c
        } else {
            a = CWPM_Add_Absolute_Url(c, a)
        }
        if (0 < b.length) {
            return CWPM_Add_Absolute_Url(b, a)
        }
    } else {
        if (0 < b.length) {
            return CWPM_Add_Absolute_Url(b, a)
        } else {
            a = CWPM_API_Get_CodeBase(a)
        }
    }
    return a
}
function CWPM_Munge_Cabbase(f) {
    var a = CWPM_Check_Str(f);
    var g = a.split(/,/);
    a = "";
    var c;
    for (i = 0; i < g.length; i++) {
        var e = g[i];
        if (e.match(/^\s*(\/|https?:\/\/)/)) {
            g[i] = CWPM_Url(g[i]);
            g[i] = g[i].replace(/,/g, "_")
        } else {
            if (".." == e.substring(0, 2)) {
                var d = CWPM_Absolute_CodeBase_Url(g_cwpmUrl, "");
                var b = g[i];
                if (d) {
                    if (!d.match(/\/$/)) {
                        d = CWPM_API_Get_CodeBase(d)
                    }
                    while (e.match(/^\.\.\//)) {
                        d = d.replace(/\/([^\/]+\/)$/, "/");
                        e = e.substring(3)
                    }
                    b = d + e
                }
                g[i] = CWPM_Url(b, ",CT=java");
                g[i] = g[i].replace(/,/g, "_")
            }
        }
    }
    a = g.join(",");
    return a
}
function CWPM_Write_Replace(c, b) {
    var a = c.match(/javascript:/i);
    if (a) {
        return c
    }
    a = c.match(/^=\s*\'([^\\\']*)'/);
    if (a) {
        return "='" + CWPM_Url(a[1], b) + "'"
    }
    a = c.match(/^=\s*\"([^\\\"]*)\"/);
    if (a) {
        return '="' + CWPM_Url(a[1], b) + '"'
    }
    a = c.match(/^=\s*`([^\\`]*)`/);
    if (a) {
        return "=`" + CWPM_Url(a[1], b) + "`"
    }
    return "=" + CWPM_Url(c.substring(1), b)
}
function CWPM_Write(h, G, c, f, b) {
    if ("" == G || null == G) {
        return ""
    }
    var al, ak;
    var aj;
    var ai = typeof (G);
    var ap = "";
    var v = "";
    var aq = null;
    var H = 0;
    if (h && "object" == typeof (h.document)) {
        aq = h.document
    } else {
        if (h && "object" == typeof (h.ownerDocument)) {
            aq = h.ownerDocument
        }
    }
    if (null != aq && "string" == (typeof (aq.designMode)) && "on" == aq.designMode.toLowerCase() && CWPM_Check_Doc(aq, true)) {
        return G
    }
    if ("string" != ai) {
        if (("object" == ai || "number" == ai) && null != G && "function" == typeof (G.toString)) {
            G = G.toString()
        } else {
            return G
        }
    }
    if (G.indexOf("javascript/cwpmjs.js") >= 0) {
        return G
    }
    if (g_CwpmTrivialWrite) {
        return CWPM_Tag_Write(G)
    }
    if (G.length > 14336) {
        var am = null;
        var z = new Array();
        var V = 0;
        var ao = 0;
        while (G.length - ao > 14000) {
            am = G.substring(ao, ao + 13388);
            z[V++] = CWPM_Write(h, am, 1, f);
            ao += 13388;
            h.WriteCwpmPrelude = 0
        }
        G = G.substring(ao, G.length);
        z[V++] = CWPM_Write(h, G, c, f, b);
        return z.join("")
    }
    if (1 == b) {
        ap = "<br/>"
    }
    if ("undefined" == typeof (f) && 0 < g_SVNSaved.length) {
        G = g_SVNSaved + G;
        g_SVNSaved = ""
    }
    var N = 0;
    var C = 0;
    var X = false;
    var I = true;
    var z = new Array();
    var V = 0;
    if (0 == G.indexOf("<<", N)) {
        return G
    }
    while (0 <= (al = G.indexOf("<", N)) || X) {
        if (X) {
            if (N > 0) {
                z[V++] = G.substring(0, N);
                G = G.substring(N)
            }
            if (!c) {
                return z.join("") + G
            }
            if ("undefined" == typeof (f)) {
                g_SVNSaved = G + ap
            }
            X = false;
            return z.join("")
        }
        if (0 == al) {
            al = N
        }
        if (0 < al) {
            z[V++] = G.substring(0, al);
            G = G.substring(al)
        }
        N = 0;
        if ("!" == G.charAt(1) && "<!--" == G.substring(0, 4)) {
            if (!G.match(/^<!--\[if[ ]+/)) {
                al = G.indexOf("-->");
                if (0 < al) {
                    N = al;
                    continue
                }
                X = true;
                continue
            }
        }
        if (CWPM_Check_IEVersion(7)) {
            if (("object" == typeof (h) && "boolean" != typeof (h.CwpmIsBaseSet) && "boolean " != typeof (h.CwpmCanBaseSet)) || ("object" != typeof (h) && !g_isBaseSet && g_canBaseSet)) {
                CWPM_Check_CanSet(h, G)
            }
            var y = G.substring(0, 6).toLowerCase();
            if ("object" != typeof (obj) && -1 != y.indexOf("</html")) {
                g_canBaseSet = true;
                g_isBaseSet = false
            }
        }
        var Q = 0;
        var O = 0;
        var M = 0;
        var p = g_TagRegx.exec(G);
        var ah = false;
        var U = G.indexOf("\n");
        if (U == G.length - 1) {
            if (p[0].length == G.length - 1) {
                ah = true
            }
        }
        if (p) {
            if (">" == p[7] || true == ah) {
                O = p[0].length;
                M = p[1].length + 1;
                if (p[3] && 0 < p[3].length) {
                    if ("/" == p[3]) {
                        Q = 3
                    } else {
                        Q = 1
                    }
                } else {
                    Q = 2
                }
                O = p[0].length;
                if (ah == true) {
                    ah = false
                }
            } else {
                if ("<" == p[7]) {
                    Q = 4;
                    O = p[0].length - 1
                }
            }
        }
        if (0 == Q || 4 == Q || 1 == Q && 0 == O) {
            X = true;
            continue
        }
        if (3 == Q) {
            N = O;
            continue
        }
        var l = p[1].toLowerCase();
        if ("/" == l.charAt(0)) {
            if ("/html" == l && "undefined" != typeof (DSTBSettings) && 0 < DSTBSettings) {
                z[V++] = G.substring(0, O);
                if (h && !("string" == typeof (h.tagName) && "DIV" == h.tagName)) {
                    z[V++] = '</noscript><SCRIPT language="javascript" id="dstb-id">if(typeof(dstb)!="undefined"){dstb();}</SCRIPT>'
                }
                G = G.substring(O);
                N = 0;
                continue
            }
        } else {
            if ("html" == l || "frameset" == l) {
                while (true) {
                    if (!I || !h || ("unknown" != typeof (document) && h == document) || "undefined" != typeof (h.CwpmPrelude)) {
                        break
                    }
                    if (!CWPM_Check_Doc(h)) {
                        var ad = null;
                        if ("undefined" != typeof (h.document)) {
                            ad = h.document
                        } else {
                            if ("undefined" != typeof (h.ownerDocument)) {
                                ad = h.ownerDocument
                            }
                        }
                        if (CWPM_Check_Doc(ad) && ad.getElementById("svnfunc")) {
                            I = false;
                            break
                        }
                    }
                    break
                }
                if (h && "unknown" != typeof (document) && h == document && "unknown" != typeof (h.readyState) && "complete" == h.readyState) {
                    h.CwpmPrelude = 0
                }
                if (I && h && !h.CwpmPrelude) {
                    if (h.WriteCwpmPrelude) {
                        var ab = CWPM_Prelude(h);
                        h.write(z.join("") + ab);
                        h.WriteCwpmPrelude = 0;
                        z = new Array();
                        V = 0;
                        z[V++] = G.substring(0, O)
                    } else {
                        z[V++] = CWPM_Prelude(h) + G.substring(0, O)
                    }
                    h.CwpmPrelude = 1;
                    I = false;
                    G = G.substring(O);
                    continue
                }
            } else {
                if ("body" == l || "head" == l || "script" == l) {
                    while (true) {
                        if (!I || !h || "unknown" != (typeof (document) && h == document) || h.CwpmPrelude) {
                            break
                        }
                        if (!CWPM_Check_Doc(h)) {
                            var ad = null;
                            if ("undefined" != typeof (h.document)) {
                                ad = h.document
                            } else {
                                if ("undefined" != typeof (h.ownerDocument)) {
                                    ad = h.ownerDocument
                                }
                            }
                            if (CWPM_Check_Doc(ad) && ad.getElementById("SVNfunc")) {
                                I = false;
                                break
                            }
                        }
                        break
                    }
                    if (h && "unknown" != typeof (document) && h == document && "unknown" != typeof (h.readyState) && "complete" == h.readyState) {
                        h.CwpmPrelude = 0
                    }
                    if (I && h && !h.CwpmPrelude) {
                        if (h.WriteCwpmPrelude) {
                            var ab = CWPM_Prelude(h);
                            h.write(z.join("") + ab);
                            if (h.WriteCwpmPrelude) {
                                h.WriteCwpmPrelude = 0
                            }
                            z = new Array();
                            V = 0
                        } else {
                            z[V++] = CWPM_Prelude(h)
                        }
                        h.CwpmPrelude = 1;
                        I = false
                    }
                    if ("script" == l) {
                        Q = 1
                    }
                } else {
                    if ("frame" == l || "iframe" == l || "style" == l) {
                        Q = 1
                    }
                }
            }
        }
        var u = O;
        var F = true;
        if (c && 4 < l.length) {
            var q;
            F = false;
            switch (l) {
                case "applet":
                    q = G.match(/(<\/applet|<\/APPLET)/i);
                    break;
                case "object":
                    q = G.match(/(<\/object|<\/OBJECT)/i);
                    break;
                case "script":
                    q = G.match(/(<\/script|<\/SCRIPT)/i);
                    break;
                case "style":
                    q = G.match(/(<\/style|<\/STYLE)/i);
                    break;
                default:
                    F = true
            }
            if (!F && !q) {
                X = true;
                continue
            }
        }
        if (F && 2 == Q || "/" == l.charAt(0)) {
            N = O;
            continue
        }
        var ag = false;
        var au = false;
        var T = 0;
        var R = 0;
        switch (l) {
            case "a":
            case "area":
            case "body":
            case "button":
            case "div":
            case "fig":
            case "form":
            case "image":
            case "img":
            case "v:imagedata":
            case "input":
            case "select":
            case "span":
            case "table":
            case "td":
            case "ul":
            case "ol":
            case "tr":
                break;
            case "base":
                R = 6;
                break;
            case "style":
                T = 3;
                break;
            case "link":
                R = 7;
                break;
            case "frameset":
            case "ilayer":
            case "layer":
            case "xml":
                break;
            case "meta":
                R = 3;
                break;
            case "embed":
                if (-1 != g_SVNaxflag) {
                    g_SVNaxflag = 1
                }
                R = 4;
                break;
            case "frame":
            case "iframe":
                ag = true;
                R = 8;
                break;
            case "object":
                if (-1 != g_SVNaxflag) {
                    g_SVNaxflag = 1
                }
                T = 5;
                K = "";
                R = 2;
                break;
            case "script":
                T = 2;
                E = 1;
                W = 0;
                R = 5;
                break;
            case "applet":
                if (-1 != g_SVNaxflag) {
                    g_SVNaxflag = 1
                }
                T = 4;
                R = 2;
                break;
            case "time:audio":
            case "t:audio":
            case "time:img":
            case "t:img":
            case "time:media":
            case "t:media":
            case "time:video":
            case "t:video":
            case "time:animation":
            case "t:animation":
            case "time:ref":
            case "t:ref":
                R = 1;
                break;
            default:
                N = u;
                continue
        }
        var Y = G.substring(M, u);
        var x = 0;
        var D = "";
        var P = "";
        z[V++] = "<" + l + "";
        G = G.substring(u);
        var e = 0;
        var k = 0;
        var K = "";
        var W = 0;
        var E = 1;
        var Z = "";
        var A = "";
        while (0 < Y.length) {
            var d = g_AttrRegx.exec(Y);
            if (!d) {
                break
            }
            Y = Y.substring(d[0].length);
            var ac = d[1].toLowerCase();
            var a = d[2];
            if (ac == '\\"' || ac == "\\'") {
                continue
            }
            if (ac == "/" && !a) {
                z[V++] = d[1];
                continue
            }
            if (!a) {
                a = ""
            } else {
                if (ac.length > 5 && "o" == ac.charAt(0) && "n" == ac.charAt(1)) {
                    switch (ac) {
                        case "onmousedown":
                        case "onmousemove":
                        case "onmouseout":
                        case "onmouseover":
                        case "onmouseup":
                        case "onclick":
                        case "ondblclick":
                        case "onabort":
                        case "onblur":
                        case "onchange":
                        case "onerror":
                        case "onfocus":
                        case "onkeydown":
                        case "onkeypress":
                        case "onkeyup":
                        case "onload":
                        case "onreset":
                        case "onselect":
                        case "onsubmit":
                        case "onresize":
                        case "onunload":
                            a = CWPM_WriteJS(h, a);
                            break
                    }
                } else {
                    if (5 == ac.length && "style" == ac) {
                        a = CWPM_CSS_Url(a)
                    } else {
                        switch (R) {
                            case 0:
                                switch (l + "-" + ac) {
                                    case "a-href":
                                    case "area-href":
                                    case "body-background":
                                    case "fig-src":
                                    case "ilayer-src":
                                    case "image-src":
                                    case "img-src":
                                    case "input-src":
                                    case "layer-src":
                                    case "table-background":
                                    case "td-background":
                                    case "xml-src":
                                    case "v:imagedata-src":
                                    case "image-xlink:href":
                                        a = CWPM_Write_Replace(a, 0, 0, 1);
                                        break;
                                    case "form-action":
                                        var ae = "/^=\\s*(['\"`])/";
                                        var r = a.match(ae);
                                        if (!(r && r[1] == r[2])) {
                                            a = CWPM_Write_Replace(a, 0, 0, 1);
                                            ","
                                        }
                                        break;
                                    case "span-behavior":
                                        a = CWPM_Url(a);
                                        break
                                }
                                break;
                            case 2:
                                switch (l + "-" + ac) {
                                    case "applet-codebase":
                                        u = a.indexOf("=");
                                        a = a.replace(" ", "");
                                        var ar = a.lastIndexOf("/");
                                        var aa = a;
                                        Z = a.substring(u + 1);
                                        a = a.substring(0, u + 1) + '"' + CWPM_Url(Z.substring(1, Z.length - 1)) + '"';
                                        if (aa.length - 1 != ar) {
                                            a = a.replace("/0-0+", "");
                                            a = a.substring(0, a.length - 1) + '/0-0+"'
                                        }
                                        e = 1;
                                        break;
                                    case "object-codebase":
                                        u = a.indexOf("=");
                                        Z = a.substring(u + 1);
                                        a = a.substring(0, u + 1) + '"' + CWPM_Url(Z.substring(1, Z.length - 1)) + '"';
                                        e = 1;
                                        break;
                                    case "object-data":
                                        u = a.indexOf("=");
                                        Z = a.substring(u + 1);
                                        a = a.substring(0, u + 1) + CWPM_Url(CWPM_Check_Str(Z));
                                        k = 1;
                                        break;
                                    case "applet-archive":
                                        u = a.indexOf("=");
                                        v = CWPM_Munge_Cabbase(a.substring(u + 1));
                                        a = a.substring(0, u + 1) + v;
                                        break;
                                    case "object-classid":
                                        u = a.indexOf("=");
                                        K = a.substring(u + 1);
                                        break
                                }
                                break;
                            case 6:
                                switch (l + "-" + ac) {
                                    case "base-href":
                                        A = a;
                                        a = CWPM_Write_Replace(a);
                                        var t = CWPM_Check_IEVersion(7);
                                        if (!t || (t && ((typeof (h) == "object" && typeof (h.CwpmIsBaseSet) != "boolean " && typeof (h.CwpmCanBaseSet) != "boolean") || (typeof (h) != "object" && !g_isBaseSet && g_canBaseSet)))) {
                                            g_CwpmInfoCached = false;
                                            CWPM_Cache_CwpmInfo(CWPM_Check_Str(a.substring(1)));
                                            if (typeof (h.expando) == "boolean") {
                                                h.expando = true;
                                                h.CwpmIsBaseSet = true;
                                                h.CwpmBaseUrl = g_cwpmUrl
                                            } else {
                                                g_isBaseSet = true
                                            }
                                            T = 1
                                        }
                                        break
                                }
                                break;
                            case 1:
                            case 4:
                                if (6 == ac.length && ac.match(/qtnext/)) {
                                    ac = "qtnext"
                                }
                                if (7 == ac.length && ac.match(/hotspot/)) {
                                    ac = "hotspot"
                                }
                                switch (l + "-" + ac) {
                                    case "embed-src":
                                    case "embed-qtsrc":
                                    case "embed-href":
                                    case "embed-pluginspage":
                                    case "embed-hotspot":
                                    case "embed-qtnext":
                                    case "time:audio-src":
                                    case "t:audio-src":
                                    case "time:img-src":
                                    case "t:img-src":
                                    case "time:media-src":
                                    case "t:media-src":
                                    case "time:video-src":
                                    case "t:video-src":
                                    case "time:animation-src":
                                    case "t:animation-src":
                                    case "time:ref-src":
                                    case "t:ref-src":
                                        a = CWPM_Write_Replace(a);
                                        break;
                                    case "embed-codebase":
                                    case "embed-java_codebase":
                                        u = a.indexOf("=");
                                        Z = a.substring(u + 1);
                                        a = a.substring(0, u + 1) + CWPM_CodeBase_Replace(Z);
                                        e = 1;
                                        break;
                                    case "embed-archive":
                                    case "embed-java_archive":
                                        u = a.indexOf("=");
                                        a = a.substring(0, u + 1) + CWPM_Munge_Cabbase(a.substring(u + 1));
                                        break
                                }
                                break;
                            case 7:
                                switch (l + "-" + ac) {
                                    case "link-rel":
                                        D = CWPM_Check_Str(a.substring(1));
                                        break;
                                    case "link-type":
                                        P = CWPM_Check_Str(a.substring(1));
                                        break;
                                    case "link-href":
                                        if (D.toUpperCase() == "STYLESHEET") {
                                            var o = "";
                                            if ("" == P || P.toUpperCase() == "TEXT/CSS") {
                                                H = 5
                                            } else {
                                                if (P.toUpperCase() == "TEXT/XSL") {
                                                    H = 6
                                                }
                                            }
                                            a = CWPM_Write_Replace(a, o);
                                            D = "";
                                            P = ""
                                        } else {
                                            a = CWPM_Write_Replace(a)
                                        }
                                        break
                                }
                                break;
                            case 8:
                                switch (l + "-" + ac) {
                                    case "frame-src":
                                    case "iframe-src":
                                        au = true;
                                        var B = 0;
                                        if (typeof (DSFrameCwpmPrelude) != "undefined" && DSFrameCwpmPrelude) {
                                            B = 1
                                        }
                                        a = CWPM_Write_Replace(a, 0, B, 0, 1);
                                        break
                                }
                                break;
                            case 3:
                                switch (l + "-" + ac) {
                                    case "meta-http-equiv":
                                        if (a.match(/^=\\s*(\\\"refresh\\\"|\\\'refresh\\\'|refresh)/i)) {
                                            x = 1
                                        }
                                        break;
                                    case "meta-content":
                                        if (x) {
                                            var S = a.toUpperCase().indexOf("URL=");
                                            if (S > -1) {
                                                var g = a.substring(S + 3);
                                                if ('\\"' == g.substring(g.length - 1) || "\\'" == g.substring(g.length - 1)) {
                                                    var at = g.substring(g.length - 1);
                                                    g = g.substring(0, g.length - 1);
                                                    g = CWPM_Write_Replace(g) + at
                                                } else {
                                                    g = CWPM_Write_Replace(g)
                                                }
                                                a = a.substring(0, S + 3) + g
                                            }
                                            x = 0
                                        }
                                        break
                                }
                                break;
                            case 5:
                                switch (l + "-" + ac) {
                                    case "script-src":
                                        if (0 == H) {
                                            H = 3
                                        }
                                        a = CWPM_Write_Replace(a, H, 0, 0, 0, 1);
                                        W = 1;
                                        break;
                                    case "script-language":
                                    case "script-type":
                                        if (-1 == a.toLowerCase().indexOf("javascript") && -1 == a.toLowerCase().indexOf("jscript")) {
                                            E = 0
                                        }
                                        H = 3;
                                        break;
                                    default:
                                        break
                                }
                                break
                        }
                    }
                }
            }
            z[V++] = " " + d[1] + a + " "
        }
        if (ag && !au) {
        }
        z[V++] = Y;
        switch (T) {
            case 1:
                z[V++] = "<SCRIPT language='javascript' id='dsbaseurl'>g_cwpmUrl=\\\"\"+g_baseurl+\"\\\";</SCRIPT>";
                break;
            case 2:
                if (0 == W && 0 != E) {
                    p = G.match(/(<\/script|<\/SCRIPT)/i);
                    var J = G.match(/<script/i);
                    var w = G;
                    var L = 0;
                    if (J && p && (J.index < p.index)) {
                        L = p.index + p[0].length
                    } else {
                        if (p) {
                            L = p.index
                        }
                    }
                    while (J && p && (J.index < p.index)) {
                        w = w.substring(p.index + p[0].length);
                        p = w.match(/(<\/script)/i);
                        J = w.match(/<script/i);
                        if (p && J) {
                            L += (p.index + p[0].length)
                        } else {
                            if (p) {
                                L += p.index
                            }
                        }
                    }
                    if (!p) {
                        continue
                    }
                    var ar = 0;
                    while (" " == G.charAt(ar)) {
                        ++ar
                    }
                    if (">" == G.charAt(ar)) {
                        ++ar
                    }
                    if (ar) {
                        z[V++] = G.substring(0, ar)
                    }
                    a = G.substring(ar, L);
                    z[V++] = CWPM_Js(a);
                    G = G.substring(L)
                }
                break;
            case 3:
                p = G.match(/(<\/style>|<\/STYLE>)/i);
                if (!p) {
                    continue
                }
                a = G.substring(0, p.index);
                z[V++] = CWPM_CSS_Url(a);
                G = G.substring(p.index);
                break;
            case 4:
            case 5:
                if (4 == T) {
                    p = G.match(/(<\/applet|<\/APPLET)/i)
                } else {
                    p = G.match(/(<\/object|<\/OBJECT)/i)
                }
                if (!p) {
                    continue
                }
                a = G.substring(0, p.index);
                G = G.substring(p.index);
                if (5 == T && a.match(/<(applet|embed)/i)) {
                    a = CWPM_Write(0, a, 0, 1)
                }
                var af = CWPM_Rewrite_Param(a, K);
                z[V++] = af.yLp;
                var n = af.Jin;
                var an;
                if ("" == v) {
                    v = af.paramArchiveUrl
                }
                break
        }
    }
    return z.join("") + G
}
function CWPM_Set_DSHost() {
    if (typeof (document) != "object" || typeof (document.expando) == "unknown") {
        return
    }
    document.expando = true;
    if (typeof (DSHost) != "undefined" && typeof (document.DSHost) == "undefined") {
        document.DSHost = DSHost
    }
    return
}
function CWPM_Is_JVMClassId(c) {
    var a;
    if (a = c.match(/clsid:([A-Za-z0-9\\-]+)/i)) {
        for (var b = 0; b < CWPMJVMClassIds.length; b++) {
            if (CWPMJVMClassIds[b] && CWPMJVMClassIds[b] == a[1]) {
                return true
            }
        }
        if (a[1].match(/^CAFEEFAC-/) && a[1].match(/-ABCDEFFEDCBA$/i)) {
            return true
        }
    } else {
        if (a = c.match(/java:/i)) {
            return true
        }
    }
    return false
}
function CWPM_Rewrite_Param(u, n) {
    var k = u;
    var d = k;
    var c = "";
    var b;
    var a;
    var f;
    var p = 0;
    var v;
    var h = "";
    var j = null;
    var y = "";
    if (a = n.match(/clsid:([A-Za-z0-9\\-]+)/i)) {
        for (v = 0; v < CwpmActiveXParams.length; v++) {
            if (CwpmActiveXParams[v] && a[1].toUpperCase() == CwpmActiveXParams[v][0]) {
                p = CwpmActiveXParams[v][1];
                break
            }
        }
    }
    while (a = k.match(/<param /i)) {
        f = k.indexOf(a[0]);
        var g = "";
        c += k.substring(0, f);
        g += k.substring(f, f + a[0].length);
        k = k.substring(f + a[0].length);
        a = k.match(/^(\s*name\s*=\s*(\S+)\s+(value\s*=\s*))(((\\\'[^\\\']+\\\')|(\\\"[^\\\"]+\\\")|([^\\\'\\\">\s][^<>\s]*)))?([^\\>\\<]*>?)/i);
        if (!a) {
            c += g;
            continue
        }
        f = k.indexOf(a[0]);
        g += k.substring(0, f + a[1].length);
        var t = "";
        if (a[4]) {
            t = a[4]
        }
        b = k.substring(f + a[1].length, f + a[1].length + t.length);
        k = k.substring(f + a[0].length);
        if (a[2].match(/useslibrary/i)) {
            if (a[2].match(/useslibrarycodebase/i)) {
                var e = CWPM_Check_Str(t);
                if (e.substring(0, 1) == "/") {
                    e = CWPM_Url(e, ",CT=java");
                    e = e.replace(/,/g, "_")
                }
                g = '<param name="cabbase" value="' + e + '"' + a[9]
            } else {
                g = ""
            }
            c += g;
            continue
        } else {
            if (a[2].match(/cabbase|cabinet|archive|java_archive/i)) {
                e = CWPM_Munge_Cabbase(t);
                g = "<param name=" + a[2] + " value=" + e + a[9];
                y = e;
                c += g;
                continue
            } else {
                if (p) {
                    var l = CWPM_Check_Str(a[2]);
                    for (v = 0; v < p.length; v++) {
                        if (l.toLowerCase() == p[v][0].toLowerCase()) {
                            switch (p[v][1]) {
                                case 1:
                                    b = CWPM_Check_Str(b);
                                    b = CWPM_Url(b);
                                    break;
                                case 2:
                                    var r;
                                    if (r = CWPM_Get_Location().href.match((/^(https?:\/\/)?([^\/:]+)/))) {
                                        b = r[0]
                                    }
                                    break;
                                case 3:
                                    b = CWPM_Check_Str(b);
                                    b = CWPM_Url(b);
                                    break;
                                case 9:
                                    var m = CWPM_Check_Str(b);
                                    m = m.toLowerCase();
                                    if (m == "http") {
                                        b = b.replace(/http/, "https")
                                    }
                                    break
                            }
                            break
                        }
                    }
                }
            }
        }
        var o = (a[9].indexOf('"') >= 0) ? '"' : ((a[9].indexOf("'") >= 0) ? "'" : "");
        var f;
        var q = a[9].indexOf("=");
        var w = a[9].length;
        if (o != "") {
            q = a[9].indexOf(o);
            w = a[9].lastIndexOf(o)
        }
        if (a[2].match(/movie/i) || a[2].match(/transparent/i)) {
            var x = CWPM_Url(a[9].substring(q + 1, w));
            a[9] = a[9].substring(q, q + 1) + x + a[9].substring(w)
        }
        if (a[2].match(/^[\'\"]?codebase/i) || a[2].match(/^[\'\"]?java_codebase/i)) {
            var x = CWPM_Url(a[9].substring(q + 1, w));
            a[9] = a[9].substring(q, q + 1) + x + a[9].substring(w)
        }
        if (n.match(/^[\'\"]?clsid:8AD9C840-044E-11D1-B3E9-00805F499D93[\'\"]?/i) && a[2].match(/^[\'\"]?jsMethodName/i)) {
            var x = CWPM_Url(a[9].substring(q + 1, w));
            a[9] = a[9].substring(q, q + 1) + x + a[9].substring(w)
        }
        c += g + b + a[9]
    }
    d = CWPM_Write(0, k, 0, true);
    return {yLp: c + d,Jin: h,paramArchiveUrl: y}
}
function CWPM_Check_Equal(b, a) {
    if ("string" != typeof (b) || "string" != typeof (a)) {
        return false
    }
    return b.toLowerCase() == a.toLowerCase()
}
var CwpmWriteCode = 0;
function CWPM_Get_Code() {
    if ("undefined" != typeof (CwpmCode)) {
        return CwpmCode
    }
    if ("undefined" != typeof (parent) && "undefined" != typeof (parent.dsGetDanaCode) && parent != top) {
        return parent.CWPM_Get_Code()
    }
    if ("undefined" != typeof (top.CwpmCode)) {
        return top.CwpmCode
    }
    if ("undefined" != typeof (opener) && "unknown" != typeof (opener.closed) && !opener.closed && "undefined" != typeof (opener.CWPM_Get_Code)) {
        return opener.CWPM_Get_Code()
    }
    return ""
}
function CWPM_Prelude(e) {
    var d = "";
    if (typeof (window) == "object" && (typeof (window.document) == "object" || typeof (window.location) == "object")) {
        if (typeof (window.document.URL) != "undefined") {
            d = window.document.URL
        } else {
            d = window.location + ""
        }
    } else {
        if (typeof (top) == "object" && typeof (top.document) == "object") {
            d = top.document.URL
        }
    }
    var a = d.match(/\//g);
    var c = "";
    for (var b = 0; b < a.length - 3; b++) {
        c += "../"
    }
    return '<script  type="text/javascript" src="' + c + 'javascript/cwpmjs.js"><\/script><script  type="text/javascript" src="' + c + 'javascript/wpmconfig.js"><\/script>'
}
function CWPM_WriteJS(f, h) {
    if (1 == CWPM_Check_DHTMLMode()) {
        return h
    }
    var c = h.match(/^=\s*\'([^\']*)\'/);
    var g;
    var b;
    if (c) {
        b = "'";
        g = c[1]
    } else {
        if ((c = h.match(/^=\s*\"([^\"]*)\"/))) {
            b = '"';
            g = c[1]
        } else {
            g = h.substring(1)
        }
    }
    var d = "";
    if ("vbscript" == g_CwpmFirstScript) {
        if (!((g.match(/^\s*jscript:/i)) || (g.match(/^\s*javascript:/i)) || (g.match(/^\s*vbscript:/i)))) {
            d = "vbscript:"
        }
    }
    var e = CWPM_Expand_CharRef(f, g);
    if (e[0]) {
        g = e[1]
    }
    var a;
    if (('"' == b) || ("'" == b)) {
        a = CWPM_Js(g, 1)
    } else {
        a = CWPM_Js(g)
    }
    if (e[0]) {
        a = a.replace(/&/g, "&amp;").replace(/\"/g, "&quot;").replace(/</g, "&lt;").replace(/>/g, "&gt;").replace(/\'/g, "&#39;")
    }
    if ("vbscript:" == d && a.match(/return /)) {
        d = ""
    }
    if ("undefined" == typeof (b)) {
        b = '"';
        if (-1 != a.indexOf('"')) {
            b = "'"
        }
        return "=" + b + d + a + b
    }
    return "=" + b + d + a + b
}
function CWPM_Expand_CharRef(d, e) {
    var a = [false, ""];
    if (0 > e.indexOf("&")) {
        return a
    }
    var b;
    if (CWPM_Check_Doc(d)) {
        b = d.createElement("div")
    } else {
        if (d && CWPM_Check_Doc(d.document)) {
            b = d.document.createElement("div")
        } else {
            b = document.createElement("div")
        }
    }
    e = e.replace(/</g, "&lt;").replace(/>/g, "&gt;");
    b.innerHTML = e;
    var c = b.innerHTML;
    c = c.replace(/&lt;/g, "<").replace(/&amp;/g, "&").replace(/&gt;/g, ">");
    b = null;
    a[0] = true;
    a[1] = c;
    return a
}
function CWPM_Call_Method(arr) {
    var obj = arr[1];
    switch (arr.length) {
        case 2:
            return obj[arr[0]]();
        case 3:
            return obj[arr[0]](arr[2]);
        case 4:
            return obj[arr[0]](arr[2], arr[3]);
        case 5:
            return obj[arr[0]](arr[2], arr[3], arr[4]);
        case 6:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5]);
        case 7:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5], arr[6]);
        case 8:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5], arr[6], arr[7]);
        case 9:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8]);
        case 10:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9]);
        case 11:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9], arr[10]);
        case 12:
            return obj[arr[0]](arr[2], arr[3], arr[4], arr[5], arr[6], arr[7], arr[8], arr[9], arr[10], arr[11]);
        default:
            var strFunc = "" + obj + "[" + arr[0] + "](";
            var i;
            for (i = 2; i < arr.length; i++) {
                if (i > 2) {
                    strFunc += ","
                }
                strFunc += arr[i]
            }
            strFunc += ");";
            return eval(strFunc)
    }
}
function CWPM_Call_Func(args) {
    var funcName = args[0];
    var str;
    switch (args.length) {
        case 1:
            return funcName();
        case 2:
            return funcName(args[1]);
        case 3:
            return funcName(args[1], args[2]);
        case 4:
            return funcName(args[1], args[2], args[3]);
        case 5:
            return funcName(args[1], args[2], args[3], args[4]);
        case 6:
            return funcName(args[1], args[2], args[3], args[4], args[5]);
        case 7:
            return funcName(args[1], args[2], args[3], args[4], args[5], args[6]);
        case 8:
            return funcName(args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
        case 9:
            return funcName(args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8]);
        case 10:
            return funcName(args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9]);
        case 11:
            return funcName(args[1], args[2], args[3], args[4], args[5], args[6], args[7], args[8], args[9], args[10]);
        default:
            var strFunc = "" + funcName + "(";
            var i;
            for (i = 1; i < args.length; i++) {
                if (i > 1) {
                    strFunc += ","
                }
                strFunc += args[i]
            }
            strFunc += ");";
            return eval(strFunc)
    }
}
var g_Cwpm_Random_Num = 32;
var g_Cwpm_Domain_Len = 256;
var g_SYS_RESOURCE_PATHNAME_LEN = 128;
var g_Cwpm_Upper_Word_Len = 26;
var g_Cwpm_Lower_Word_Len = 26;
var g_Cwpm_Digital_len = 10;
var g_Cwpm_Other_Len = 2;
var g_Cwpm_English_Word_len = g_Cwpm_Upper_Word_Len + g_Cwpm_Lower_Word_Len;
var g_Cwpm_Domain_Charset_Len = (g_Cwpm_English_Word_len + g_Cwpm_Digital_len + g_Cwpm_Other_Len);
var g_Random_Fixed = 250;
var g_Cwpm_Random_Flag_Odd = 1;
var g_Cwpm_Random_flag_Even = 0;
var g_Cwpm_Cre_Kmp_Error = -1;
var g_Cwpm_Cre_Current_Symbol = "*";
var g_Cwpm_Cre_End_Of_Str = "\0";
var g_Cwpm_Cre_Absurl_Symbol = "//";
var g_Cwpm_Cre_Absurl_Symbol_Len = 2;
var g_VOS_OK = 0;
var domainCharsetArr = CWPM_Arrs(g_Cwpm_Domain_Charset_Len);
var oddRandNumArr = CWPM_TwoArrs();
var evenRandNum = CWPM_TwoArrs();
var aucDomain = CWPM_Arrs(g_Cwpm_Domain_Len);
function tagRTMCtx(a, b, c) {
    this.aucWPMOddRandNum = a;
    this.aucWPMEvenRandNum = b;
    this.aucWPMDomainCharSet = c
}
var g_pstSOSRTMCTX = new tagRTMCtx(oddRandNumArr, evenRandNum, domainCharsetArr);
function CWPM_EncodeInit() {
    var a = 0;
    var b = 0;
    var c = 0;
    var d = 0;
    for (a = 0; a < g_Cwpm_Random_Num; a++) {
        for (b = 0; b < g_Cwpm_Domain_Len; b++) {
            if (0 == b) {
                c = g_Random_Fixed % g_Cwpm_Domain_Charset_Len;
                g_pstSOSRTMCTX.aucWPMOddRandNum[a][b] = parseInt(c / g_Cwpm_Random_Num) * g_Cwpm_Random_Num
            } else {
                if (1 == b % 2) {
                    g_pstSOSRTMCTX.aucWPMOddRandNum[a][b] = g_Random_Fixed % g_Cwpm_Domain_Charset_Len
                } else {
                    c = g_Random_Fixed % g_Cwpm_Domain_Charset_Len;
                    d = g_pstSOSRTMCTX.aucWPMOddRandNum[a][b - 1];
                    c = parseInt((d + c) / g_Cwpm_Random_Num) * g_Cwpm_Random_Num;
                    if (c > d) {
                        g_pstSOSRTMCTX.aucWPMOddRandNum[a][b] = c - d
                    } else {
                        g_pstSOSRTMCTX.aucWPMOddRandNum[a][b] = c - d + g_Cwpm_Random_Num
                    }
                }
            }
        }
    }
    for (a = 0; a < g_Cwpm_Random_Num; a++) {
        for (b = 0; b < g_Cwpm_Domain_Len; b++) {
            if (0 == b % 2) {
                g_pstSOSRTMCTX.aucWPMEvenRandNum[a][b] = g_Random_Fixed % g_Cwpm_Domain_Charset_Len
            } else {
                c = g_Random_Fixed % g_Cwpm_Domain_Charset_Len;
                d = g_pstSOSRTMCTX.aucWPMEvenRandNum[a][b - 1];
                g_pstSOSRTMCTX.aucWPMEvenRandNum[a][b] = parseInt((d + c) / g_Cwpm_Random_Num) * g_Cwpm_Random_Num - d
            }
        }
    }
    for (b = 0; b < g_Cwpm_Lower_Word_Len; b++) {
        g_pstSOSRTMCTX.aucWPMDomainCharSet[b] = String.fromCharCode(97 + b)
    }
    for (b = 0; b < g_Cwpm_Upper_Word_Len; b++) {
        g_pstSOSRTMCTX.aucWPMDomainCharSet[b + g_Cwpm_Lower_Word_Len] = String.fromCharCode(65 + b)
    }
    for (b = 0; b < g_Cwpm_Digital_len; b++) {
        g_pstSOSRTMCTX.aucWPMDomainCharSet[b + g_Cwpm_English_Word_len] = String.fromCharCode(48 + b)
    }
    g_pstSOSRTMCTX.aucWPMDomainCharSet[g_Cwpm_English_Word_len + g_Cwpm_Digital_len] = ".";
    g_pstSOSRTMCTX.aucWPMDomainCharSet[g_Cwpm_English_Word_len + g_Cwpm_Digital_len + 1] = ":"
}
CWPM_EncodeInit();
function CWPM_UrlEncode(d, b) {
    var k = 0;
    var h = 0;
    var g = 0;
    var c = 0;
    var e = 0;
    var f = 0;
    var l = 0;
    if (null == d || 0 == b) {
        return
    }
    h = d.length;
    h = (b < h) ? b : h;
    if (g_Cwpm_Domain_Len < h) {
        alert("The length of URL is too long, don't encode it.");
        return
    }
    var a = d.join("");
    for (k = 0; k < h; k++) {
        var j = a.charCodeAt(k);
        if (97 <= j && j <= 122) {
            g = j - 97
        } else {
            if (65 <= j && j <= 90) {
                g = j - 65 + g_Cwpm_Lower_Word_Len
            } else {
                if (48 <= j && j <= 57) {
                    g = j - 48 + g_Cwpm_English_Word_len
                } else {
                    if (46 == j) {
                        g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len
                    } else {
                        if (58 == j) {
                            g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len + 1
                        } else {
                            continue
                        }
                    }
                }
            }
        }
        l++;
        c += g
    }
    e = c % g_Cwpm_Random_Num;
    if (1 == l % 2) {
        f = g_Cwpm_Random_Flag_Odd
    } else {
        f = g_Cwpm_Random_flag_Even
    }
    l = 0;
    for (k = 0; k < h; k++) {
        var j = a.charCodeAt(k);
        if (97 <= j && j <= 122) {
            g = j - 97
        } else {
            if (65 <= j && j <= 90) {
                g = j - 65 + g_Cwpm_Lower_Word_Len
            } else {
                if (48 <= j && j <= 57) {
                    g = j - 48 + g_Cwpm_English_Word_len
                } else {
                    if (46 == j) {
                        g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len
                    } else {
                        if (58 == j) {
                            g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len + 1
                        } else {
                            continue
                        }
                    }
                }
            }
        }
        if (g_Cwpm_Random_Flag_Odd == f) {
            g = (g + g_pstSOSRTMCTX.aucWPMOddRandNum[e][l]) % g_Cwpm_Domain_Charset_Len;
            d[k] = g_pstSOSRTMCTX.aucWPMDomainCharSet[g]
        } else {
            g = (g + g_pstSOSRTMCTX.aucWPMEvenRandNum[e][l]) % g_Cwpm_Domain_Charset_Len;
            d[k] = g_pstSOSRTMCTX.aucWPMDomainCharSet[g]
        }
        l++
    }
    return
}
function CWPM_UrlDecode(c, f) {
    var k = 0;
    var h = 0;
    var g = 0;
    var b = 0;
    var d = 0;
    var e = 0;
    var l = 0;
    if ((null == c) || (0 == f)) {
        return
    }
    h = c.length;
    h = (f < h) ? f : h;
    if (g_Cwpm_Domain_Len < h) {
        return
    }
    b = 0;
    l = 0;
    var a = c.join("");
    for (k = 0; k < h; k++) {
        var j = a.charCodeAt(k);
        if (97 <= j && j <= 122) {
            g = j - 97
        } else {
            if (65 <= j && j <= 90) {
                g = j - 65 + g_Cwpm_Lower_Word_Len
            } else {
                if (48 <= j && j <= 57) {
                    g = j - 48 + g_Cwpm_English_Word_len
                } else {
                    if (46 == j) {
                        g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len
                    } else {
                        if (58 == j) {
                            g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len + 1
                        } else {
                            continue
                        }
                    }
                }
            }
        }
        l++;
        b += g
    }
    d = b % g_Cwpm_Random_Num;
    if (1 == l % 2) {
        e = g_Cwpm_Random_Flag_Odd
    } else {
        e = g_Cwpm_Random_flag_Even
    }
    l = 0;
    for (k = 0; k < h; k++) {
        var j = a.charCodeAt(k);
        if ((97 <= j) && (j <= 122)) {
            g = j - 97
        } else {
            if (65 <= j && j <= 90) {
                g = j - 65 + g_Cwpm_Lower_Word_Len
            } else {
                if (48 <= j && j <= 57) {
                    g = j - 48 + g_Cwpm_English_Word_len
                } else {
                    if (46 == j) {
                        g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len
                    } else {
                        if (58 == j) {
                            g = g_Cwpm_English_Word_len + g_Cwpm_Digital_len + 1
                        } else {
                            continue
                        }
                    }
                }
            }
        }
        if (g_Cwpm_Random_Flag_Odd == e) {
            g = g - g_pstSOSRTMCTX.aucWPMOddRandNum[d][l];
            while (g < 0) {
                g = g + g_Cwpm_Domain_Charset_Len
            }
            c[k] = g_pstSOSRTMCTX.aucWPMDomainCharSet[g]
        } else {
            g = g - g_pstSOSRTMCTX.aucWPMEvenRandNum[d][l];
            while (g < 0) {
                g = g + g_Cwpm_Domain_Charset_Len
            }
            c[k] = g_pstSOSRTMCTX.aucWPMDomainCharSet[g]
        }
        l++
    }
    return
}
function CWPM_Arr(c, d) {
    var a = new Array();
    for (var b = 0; b < c.length; b++) {
        a[b] = c.charAt(b);
        d[b] = a[b]
    }
    return a
}
function CWPM_ArrByStr(c) {
    var a = new Array();
    for (var b = 0; b < c.length; b++) {
        a[b] = c.charAt(b)
    }
    return a
}
function CWPM_TwoArrs() {
    var c = new Array();
    for (var b = 0; b < g_Cwpm_Random_Num; b++) {
        c[b] = new Array();
        for (var a = 0; a < g_Cwpm_Domain_Len; a++) {
            c[b][a] = 0
        }
    }
    return c
}
function CWPM_Arrs(b) {
    var c = new Array();
    for (var a = 0; a < b; a++) {
        c[a] = 0
    }
    return c
}
function CWPM_Compare(m, d) {
    var o = false;
    var b = d.join("");
    var a = b.match(/srcUrl/);
    if (a) {
        return true
    } else {
        var e = m.split(".");
        for (var h = 0; h < d.length; h++) {
            if ("undefined" == typeof (d[h])) {
                continue
            }
            var p = d[h].split(".");
            for (var f = 0; f < p.length; f++) {
                if ("*" == p[f] || p[f] == e[f]) {
                    o = true;
                    continue
                } else {
                    var l = e[f].split("");
                    var g = p[f].split("");
                    var k = 0;
                    for (var c = 0; c < g.length; c++) {
                        if ("*" == g[c] || (g[c] == l[c] && g.length == l.length)) {
                            o = true;
                            continue
                        }
                        if (c > k) {
                            k = c
                        }
                        for (; k < l.length; ) {
                            if (g[c] == l[k]) {
                                if (0 != c && g[c - 1] != l[k - 1] && "*" != g[c - 1]) {
                                    o = false;
                                    break
                                } else {
                                    if (0 == c && 0 <= k - 1 && g[c] != l[k - 1] && "*" != g[c - 1]) {
                                        o = false;
                                        break
                                    } else {
                                        if (k + 1 == l.length && c + 1 == g.length) {
                                            o = true;
                                            break
                                        } else {
                                            if (k + 1 < l.length && c + 1 == g.length) {
                                                o = false;
                                                break
                                            } else {
                                                if (k + 1 == l.length && c + 1 < g.length) {
                                                    o = false;
                                                    break
                                                }
                                            }
                                        }
                                    }
                                }
                                o = true;
                                break
                            } else {
                                o = false;
                                k++
                            }
                        }
                        if (!o) {
                            break
                        }
                    }
                    if (!o) {
                        break
                    } else {
                        continue
                    }
                }
            }
            if (o) {
                break
            } else {
                continue
            }
        }
    }
    return o
}
function CWPM_SpecialFunc(c, b, d) {
    switch (b) {
        case "go":
            return CWPM_API_Go(arguments[0], arguments[2]);
        case "add":
            return CWPM_API_Add(arguments[0], arguments[2], arguments[3]);
        case "attr":
            return CWPM_API_Attr(arguments[0], arguments[2], arguments[3]);
        case "load":
            return CWPM_API_Load(arguments[0], arguments[2]);
        case "open":
            return CWPM_API_Open(arguments[0], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6]);
        case "clear":
            return CWPM_API_Clear(arguments[0]);
        case "close":
            return CWPM_API_Close(arguments[0]);
        case "write":
            return CWPM_API_Write(arguments[0], arguments[2]);
        case "assign":
            return CWPM_API_Assign(arguments[0], arguments[2]);
        case "reload":
            return CWPM_API_Reload(arguments[0], arguments[2]);
        case "addRule":
            return CWPM_API_AddRule(arguments[0], arguments[2], arguments[3]);
        case "loadURL":
            return CWPM_API_LoadURL(arguments[0], arguments[2]);
        case "refresh":
            return CWPM_API_Refresh(arguments[0]);
        case "replace":
            return CWPM_API_Replace(arguments[0], arguments[2], arguments[3]);
        case "writeln":
            return CWPM_API_Writeln(arguments[0], arguments[2]);
        case "addParam":
            return CWPM_API_AddParam(arguments[0], arguments[2], arguments[3]);
        case "toString":
            return CWPM_API_ToString(arguments[0]);
        case "setTimeout":
            return CWPM_API_SetTimeout(arguments[0], arguments[2], arguments[3], arguments[4]);
        case "addBehavior":
            return CWPM_API_AddBehavior(arguments[0], arguments[2]);
        case "appendChild":
            return CWPM_API_AppendChild(arguments[0], arguments[2]);
        case "execCommand":
            return CWPM_API_ExecCommand(arguments[0], arguments[2]);
        case "setInterval":
            return CWPM_API_SetInterval(arguments[0], arguments[2], arguments[3]);
        case "createObject":
            return CWPM_API_CreateObject(arguments[0], arguments[2], arguments[3]);
        case "startDownload":
            return CWPM_API_StartDownload(arguments[0], arguments[2], arguments[3]);
        case "submitChanges":
            return CWPM_API_SubmitChanges(arguments[0]);
        case "createElement":
            return CWPM_API_CreateElement(arguments[0], arguments[2]);
        case "showModalDialog":
            return CWPM_API_ShowModalDialog(arguments[0], arguments[2], arguments[3], arguments[4]);
        case "createStyleSheet":
            return CWPM_API_CreateStyleSheet(arguments[0], arguments[2]);
        case "insertAdjacentElement":
            return CWPM_API_InsertAdjacentElement(arguments[0], arguments[2], arguments[3]);
        case "createContextualFragment":
            return CWPM_API_CreateContextualFragment(arguments[0], arguments[2]);
        case "createContextualFragment":
            return CWPM_API_CreateContextualFragment(arguments[0], arguments[2]);
        case "showHelp":
            return CWPM_API_ShowHelp(arguments[0], arguments[2], arguments[3]);
        case "insertAdjacentHTML":
            return CWPM_API_InsertAdjacentHTML(arguments[0], arguments[2], arguments[3]);
        case "showModelessDialog":
            return CWPM_API_ShowModelessDialog(arguments[0], arguments[2], arguments[3]);
        default:
            var a = arguments[0];
            arguments[0] = arguments[1];
            arguments[1] = a;
            return CWPM_Call_Method(arguments)
    }
}
function CWPM_SpecialSetProp(obj, method_name, param) {
    if ("undefined" != typeof (method_name) && "string" == typeof (method_name)) {
        method_name = arguments[1]
    } else {
        return ""
    }
    switch (method_name) {
        case "setAttribute":
            return CWPM_API_SetAttribute(arguments[0], arguments[2], arguments[3]);
        case "execCommand":
            return CWPM_API_ExecCommand(arguments[0], arguments[2]);
        case "setInterval":
            return CWPM_API_SetInterval(arguments[0], arguments[2], arguments[3]);
        case "createObject":
            return CWPM_API_CreateObject(arguments[0], arguments[2], arguments[3]);
        case "src":
            return CWPM_API_Set_Src(arguments[0], arguments[2], arguments[3]);
        case "SRC":
            return CWPM_API_Set_SRC(arguments[0], arguments[2], arguments[3]);
        case "URL":
            return CWPM_API_Set_URL(arguments[0], arguments[2], arguments[3]);
        case "href":
            return CWPM_API_Set_Href(arguments[0], arguments[2], arguments[3]);
        case "port":
            return CWPM_API_Set_Port(arguments[0], arguments[2], arguments[3]);
        case "pathname":
            return CWPM_API_Set_Pathname(arguments[0], arguments[2], arguments[3]);
        case "location":
            return CWPM_API_Set_Location(arguments[0], arguments[2], arguments[3]);
        case "action":
            return CWPM_API_Set_Action(arguments[0], arguments[2], arguments[3]);
        case "cookie":
            return CWPM_API_Set_Cookie(arguments[0], arguments[2], arguments[3]);
        case "cssText":
            return CWPM_API_Set_CssText(arguments[0], arguments[2], arguments[3]);
        case "domain":
            return CWPM_API_Set_Domain(arguments[0], arguments[2]);
        case "filter":
            return CWPM_API_Set_Filter(arguments[0], arguments[2], arguments[3], arguments[4], arguments[5]);
        case "cssText":
            return CWPM_API_Set_CssText(arguments[0], arguments[2], arguments[3]);
        case "innerHTML":
            return CWPM_API_Set_InnerHTML(arguments[0], arguments[2], arguments[3]);
        case "background":
            return CWPM_API_Set_Background(arguments[0], arguments[2], arguments[3]);
        case "backgroundImage":
            return CWPM_API_Set_BackgroundImage(arguments[0], arguments[2], arguments[3]);
        default:
            var arsLength = arguments.length;
            var strFunc = "" + arguments[0] + "[" + arguments[1] + "](";
            var i;
            for (i = 1; i <= arsLength - 1; i++) {
                if (2 == arsLength) {
                    return obj[arguments[1]]()
                }
                strFunc += arguments[i + 1] + ","
            }
            strFunc += arguments[arsLength - 1] + ");";
            return eval(strFunc)
    }
}
function CWPM_SpecialGetProp(obj, method_name) {
    if ("undefined" != typeof (method_name) && "string" == typeof (method_name)) {
        method_name = arguments[1]
    } else {
        return ""
    }
    switch (method_name) {
        case "getAttribute":
            return CWPM_API_GetAttribute(arguments[0], arguments[2]);
        case "getResponseHeader":
            return CWPM_API_GetResponseHeader(arguments[0], arguments[2]);
        case "setInterval":
            return CWPM_API_SetInterval(arguments[0], arguments[2], arguments[3]);
        case "src":
            return CWPM_API_Get_Src(arguments[0]);
        case "SRC":
            return CWPM_API_Get_SRC(arguments[0]);
        case "URL":
            return CWPM_API_Get_URL(arguments[0]);
        case "hash":
            return CWPM_API_Get_Hash(arguments[0]);
        case "host":
            return CWPM_API_Get_Host(arguments[0]);
        case "href":
            return CWPM_API_Get_Href(arguments[0]);
        case "port":
            return CWPM_API_Get_Port(arguments[0]);
        case "action":
            return CWPM_API_Get_Action(arguments[0]);
        case "cookie":
            return CWPM_API_Get_Cookie(arguments[0]);
        case "domain":
            return CWPM_API_Get_Domain(arguments[0]);
        case "filter":
            return CWPM_API_Get_Filter(arguments[0]);
        case "length":
            return CWPM_API_Get_Length(arguments[0]);
        case "search":
            return CWPM_API_Get_Search(arguments[0]);
        case "cssText":
            return CWPM_API_Get_CssText(arguments[0]);
        case "behavior":
            return CWPM_API_Get_Behavior(arguments[0]);
        case "codeBase":
            return CWPM_API_Get_CodeBase(arguments[0]);
        case "hostname":
            return CWPM_API_Get_Hostname(arguments[0]);
        case "location":
            return CWPM_API_Get_Location(arguments[0]);
        case "pathname":
            return CWPM_API_Get_Pathname(arguments[0], arguments[2]);
        case "pathname":
            return CWPM_API_Get_Pathname(arguments[0], arguments[2]);
        case "protocol":
            return CWPM_API_Get_Protocol(arguments[0]);
        case "referrer":
            return CWPM_API_Get_Referrer(arguments[0]);
        case "innerHTML":
            return CWPM_API_Get_InnerHTML(arguments[0]);
        case "background":
            return CWPM_API_Get_Background(arguments[0]);
        case "backgroundImage":
            return CWPM_API_Get_BackgroundImage(arguments[0]);
        case "childNodes":
            return CWPM_API_Get_ChildNodes(arguments[0]);
        case "firstChild":
            return CWPM_API_Get_FirstChild(arguments[0]);
        default:
            var arsLength = arguments.length;
            var strFunc = "" + arguments[0] + "[" + arguments[1] + "](";
            var i;
            for (i = 1; i <= arsLength - 1; i++) {
                if (2 == arsLength) {
                    return obj[arguments[1]]()
                }
                strFunc += arguments[i + 1] + ","
            }
            strFunc += arguments[arsLength - 1] + ");";
            return eval(strFunc)
    }
}
var changeFlag = false;
function CWPM_Set_Func_FlashUrl(a) {
    if (a != "" && null != a && a.indexOf("swfupload_f10.swf") > -1) {
        changeFlag = true;
        a = CWPM_Url(a)
    }
    return a
}
function CWPM_Set_Func_UploadUrl(a) {
    var b = "";
    if (typeof (window.document.URL) != "undefined") {
        b = window.document.URL
    } else {
        b = window.location + ""
    }
    var e = b.indexOf("/webproxy/");
    var j = b.substr(e + 10, b.length);
    var h = j.indexOf("/");
    var d = j.substr(h + 1, j.length);
    var f = d.indexOf("/");
    var c = d.substr(0, f);
    var k = c;
    var g = ',\r\n post_params : {"SessionID" : "' + k + '"},';
    if (changeFlag) {
        if (a != "" && null != a) {
            a = CWPM_Url(a);
            a = a + g
        }
    }
    return a
}
function CWPM_Set_Func_BtnImageUrl(a) {
    if (a != "" && null != a) {
        a = CWPM_Url(a)
    }
    return a
}
;
