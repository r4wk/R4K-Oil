import datetime

Import("env")

my_flags = env.ParseFlags(env['BUILD_FLAGS'])
defines = {k: v for (k, v) in my_flags.get("CPPDEFINES")}

build_tag = "R4K_"

major_ver = defines.get("MAJOR_VERSION")
minor_ver = defines.get("MINOR_VERSION")
build_date = datetime.datetime.now().strftime('%Y%m%d%H%M%S')

env.Replace(PROGNAME="../../../Generated/R4K-Oil_v%s.%sa" % (major_ver,minor_ver))
