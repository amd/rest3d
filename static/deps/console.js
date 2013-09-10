"use strict";

(function () {
CONSOLE = this.CONSOLE = {};
CONSOLE.open = function (object) {
    var here = object.jqueryObjectWest.append("<div id='support-console' style='height:100%;width:100%'></div>");
    $('#support-console').append('<div id="console" class="ui-layout-south"></div><div id="mainLayout-westBis" class="ui-layout-center"></div>')
    var newLayout = $('#support-console').layout({
        togglerLength_open: 0, // HIDE the toggler button

        south: {
            enableCursorHotkey: false,
            closable: false,
            resizable: false,
            spacing_open: 0,
            spacing_closed: 0,
        },
        center: {
            enableCursorHotkey: false,
            closable: false,
            resizable: false,
            spacing_open: 0,
            spacing_closed: 0,
            size:1000,
        }
    });

    $('#console').hide();
    $('#console').addClass('ui-widget-content');

    $('#buttonLayout-').on('change', function (e) {
        if ($(this).is(':checked')) {
            $('#console').show();
            newLayout.sizePane('south','120');
        } else {
            $('#console').hide();
            $('#support-layout').css("height", '100%');
            newLayout.sizePane('south','0');
        }
    });

    newLayout.allowOverflow("south");

    function debug() {
        allMessage();
        $('#message-console').show();
        $('#layout-console').hide();
        $('#mode_console').remove();
        GUI.label('mode_console', "debug", $('#console '));
        $('#content-console').find('div').each(function (index) {
            if (this.id == 'warn') {
                $(this).hide();
            } else if (this.id == 'log') {
                $(this).hide();
            } else if (this.id == 'error') {
                $(this).hide();
            }
        });
    };

    function log() {
        allMessage();
        $('#message-console').show();
        $('#layout-console').hide();
        $('#mode_console').remove();
        GUI.label('mode_console', "log", $('#console '));
        $('#content-console').find('div').each(function (index) {
            if (this.id == 'warn') {
                $(this).hide();
            } else if (this.id == 'debug') {
                $(this).hide();
            } else if (this.id == 'error') {
                $(this).hide();
            }
        });
    };

    function warn() {
        allMessage();
        $('#message-console').show();
        $('#layout-console').hide();
        $('#mode_console').remove();
        GUI.label('mode_console', "warning", $('#console '));
        $('#content-console').find('div').each(function (index) {
            if (this.id == 'debug') {
                $(this).hide();
            } else if (this.id == 'log') {
                $(this).hide();
            } else if (this.id == 'error') {
                $(this).hide();
            }
        });
    };

    function error() {
        allMessage();
        $('#message-console').show();
        $('#layout-console').hide();
        $('#mode_console').remove();
        GUI.label('mode_console', "error", $('#console '));
        $('#content-console').find('div').each(function (index) {
            if (this.id == 'warn') {
                $(this).hide();
            } else if (this.id == 'log') {
                $(this).hide();
            } else if (this.id == 'debug') {
                $(this).hide();
            }
        });
    };

    function allMessage() {
        $('#message-console').show();
        $('#layout-console').hide();
        $('#mode_console').remove();
        GUI.label('mode_console', "all", $('#console '));
        $('#content-console').find('div').each(function (index) {
            if (this.id == 'warn') {
                $(this).show();
            } else if (this.id == 'log') {
                $(this).show();
            } else if (this.id == 'debug') {
                $(this).show();
            } else if (this.id == 'error') {
                $(this).show();
            }
        });
    };

    window.scriptGo = function() {
        $('#message-console').hide();
        $('#layout-console').show();
        $('#mode_console').remove();
        GUI.label('mode_console', "script", $('#console '));
        $('#content-console').find('div').each(function (index) {
            if (this.id == 'warn') {
                $(this).hide();
            } else if (this.id == 'log') {
                $(this).hide();
            } else if (this.id == 'debug') {
                $(this).hide();
            } else if (this.id == 'error') {
                $(this).hide();
            }
        });
        $('#layout-console').terminal(function (command, term) {
            if (command !== '') {
                try {
                    var result = window.eval(command);
                    if (result !== undefined) {
                        term.echo(new String(result));
                    }
                } catch (e) {
                    term.error(new String(e));
                    console.error(new String(e));
                }
            } else {
                term.echo('');
            }
        }, {
            greetings: '',
            name: 'js_command',
            width: 400,
            prompt: 'js> '
        });
    };

    var linksTool = ["allMessage();", "debug();", "error();", "log();", "warn();", "window.scriptGo();"];
    var itemTool = ["icon-list", "icon-info-sign", "icon-remove-sign", "icon-ban-circle", "icon-warning-sign ", "icon-comment"];
    GUI.toolBar("toolConsole", $('#console'), itemTool, linksTool, "top", $('#normal-button-bottom'));
    //GUI.label = function(_id,_txt, _parent,_style,_mode) 
    GUI.label('title_console', "Console : ", $('#console '));
    GUI.label('mode_console', "all", $('#console '));

    $('#console').append("<div id='content-console'><div id='message-console'></div><div id='layout-console'></div></div>")
      //handling errors,warnings,debugs,logs
    //catch error 

    window.onerror = function (message, url, linenumber) {
        var content = '<div id="debug">' + GUI.time() + '<div id="blocConsole2 class="txt-debug"><a class="ui-icon ui-icon-circle-close" id="blocConsole1"></a>';
        if (!url) {
            url = "indefined";
        }
        if (!linenumber) {
            linenumber = "indefined";
        }
        content += '<a style="color:red">' + message + ' url:' + url + ' line:' + linenumber + '</a></div></div>';
        var $content = $(content);
        $('#message-console').prepend($content);
    };

    //catch debug from console firebug API
    var oldDebug = console.debug;
    console.debug = function (message) {
        var content = '<div id="debug">' + GUI.time() + '<div id="blocConsole2" class="txt-debug"><a class="ui-icon ui-icon-info" id="blocConsole1"></a>';
        content += '<a>' + message + '</a></div></div>';
        var $content = $(content);
        $('#message-console').prepend($content);
        oldDebug.apply(console, arguments);
    };

    //catch log from console firebug API
    var oldLog = console.log;
    console.log = function (message) {
        var content = '<div id="log">' + GUI.time() + '<div id="blocConsole2" class="txt-log"><a class="ui-icon ui-icon-cancel" id="blocConsole1"></a>';
        content += '<a>' + message + '</a></div></div>';
        var $content = $(content);
        $('#message-console').prepend($content);
        oldLog.apply(console, arguments);
    };

    //catch error from console firebug API
    var oldError = console.error;
    console.error = function (message) {
        var content = '<div id="error">' + GUI.time() + '<div id="blocConsole2" class="txt-error"><a class="ui-icon ui-icon-circle-close" id="blocConsole1"></a>';
        content += '<a style="color:red">' + message + '</a></div></div>';
        var $content = $(content);
        $('#message-console').prepend($content);
        oldError.apply(console, arguments);
    };

    //catch warning from console firebug API
    var oldWarn = console.warn;
    console.warn = function (message) {
        var content = '<div id="warn">' + GUI.time() + '<div id="blocConsole2" class="txt-warning"><a class="ui-icon ui-icon-alert" id="blocConsole1"></a>';
        content += '<a style="color:yellow">' + message + '</a></div></div>';
        var $content = $(content);
        $('#message-console').prepend($content);
        oldWarn.apply(console, arguments);
    };



    return
    };

}).call(this);