# nodejs servers

## static_server
  This is a http server that serve the /static/ folder 

## rest3d_server
  This is a rest3d server that uses xquery to dialog with a XML database

#### set-up for eXist-db database (note BaseX is not support currently)
+ install eXist-db 2.x either from git or stable distribution
 you'll need java 7 (jdk).
 Set JAVA_HOME to the jdk folder, such as :
     JAVA_HOME=C:\Program Files\Java\jdk1.7.0_67

+ copy the mime-type information from rest3d/database/eXist/README.md into the mime-types.xml in eXist-db. This let eXist recognize .dae files as xml content (rather than just binary data)

+ optional - changing exist html port

 the default port for exist is 8080. If you want to change it, edit tools/jetty/etc/jetty.xml, and change the port in the setting (below changed to 8081):

jetty.xml:
...
 <Set name="port"><SystemProperty name="jetty.port" default="8081"/></Set>
...

+ run exist-db. Using startup.[bat/sh] in the bin folder
+ open exist dashboard. Open browser, go to 127.0.0.1:8080 (or whatever port you decided to use)
+ create admin password
  exist has no admin password by default, some operations will not work if admin has no pasword.
  click on 'User Management', right click on 'admin', set password
+ create rest3d user
  add a user for rest3d, with a password. For example user rest3d, password rest3d

+ set-up environment variables
 the following variables will be needed, change with your specifc values

EXIST_HOME=c:\trees\eXist-db
EXIST_USER=admin
EXIST_PASSWD=admin_user_password
REST3D_USER=rest3d
REST3D_PASSWD=rest3d_user_password
EXIST_URI=xmldb:exist://localhost:8081/exist/xmlrpc

+ setup database
 This simply add a couple of collections, this can be done using a ant script.

 First, on windows, install ant:
http://dita-ot.sourceforge.net/doc/ot-userguide13/xhtml/installing/windows_installingant.html

 go to rest3d/database/eXist/ant
 > ant -f install-models.xml 
this installs a few collections with proper permissions

 > ant -f install-models.xml
this installs the 'static' models for the demos. 

+ optionnal, set-up rest3d application template

 eXist can host html applications, on its own. The rest3d project is using node as a front-end, but it may be interesting to develop an html application that runs directly in eXist-db, without node. 
 the folder rest3d/database/eXist/rest3d/ contains a template of such application. It can be installed in the esist dashboard using the install-rest3d script, or by hand using:
 type 'ant' in the eXist/rest3d/ folder. This will 'build' the rest3d application
 using the eXist web interface, bring up the package manager, click on the + sign on the harddrive symbol (upper left), upload the eXist/rest3d/build/rest3d-0.1.xar  the 'asset manager' application is now available in the dashboard.
 

+ run the server
You need nodejs. Install node js
go to rest3d/servers
install dependencies (once)
> npm install

run the server
node rest3d-server.js

open rest3d in the web browser
http://127.0.0.1:8000

note, you can change the port using the environment variable 'HTTP'



