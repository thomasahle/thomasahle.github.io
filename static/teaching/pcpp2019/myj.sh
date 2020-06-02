#!/bin/bash

CP=''
CPO=''
BASE=''
ARGSEP=''
ARGS=''

for i do
    case "$i" in
        *.jar)
            CPO='-cp .'
            CP="$CP:$i"
            ;;
        *.java)
            BASE="${i%%.java}"
            ;;
        *)
            ARGS="$ARGS$ARGSEP$i"
            ARGSEP=' '
            ;;
            
    esac
done

if [ ! -r SystemInfo.class ]
then
    cat > SystemInfo.java <<EOF
public class SystemInfo {
    
  public static void main(String argv[]) {
      SystemInfo();
  }

  public static void SystemInfo() {
    System.out.printf("# OS:   %s; %s; %s%n", 
                      System.getProperty("os.name"), 
                      System.getProperty("os.version"), 
                      System.getProperty("os.arch"));
    System.out.printf("# JVM:  %s; %s%n", 
                      System.getProperty("java.vendor"), 
                      System.getProperty("java.version"));
    // The processor identifier works only on MS Windows:
    System.out.printf("# CPU:  %s; %d \"cores\"%n", 
                      System.getenv("PROCESSOR_IDENTIFIER"),
                      Runtime.getRuntime().availableProcessors());
    java.util.Date now = new java.util.Date();
    System.out.printf("# Date: %s%n", 
      new java.text.SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ssZ").format(now));
  }
}
EOF
    javac SystemInfo.java
fi
JAVAC="javac $CPO$CP $BASE.java"
JAVA="time java $CPO$CP $BASE $ARGS"
# echo \# $JAVAC && $JAVAC && java SystemInfo && echo \# $JAVA && time -l $JAVA
echo \# $JAVAC && $JAVAC && echo \# $JAVA && $JAVA
