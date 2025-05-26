echo "🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨🔨"

if [ -d "./build" ]; then
    echo "-> 🚮 Removing old build directory..."
    rm -rf ./build/*
else
    echo "-> 👷 Creating build directory..."
    mkdir build
fi

echo "🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧"

echo "-> 📦 Installing Conan dependencies..."
conan install . --output-folder=build --build=missing

echo "-> 💻 Compiling project and 📝 building..."

cd build

cmake -GNinja -DRUN_COVERAGE=1 -DCMAKE_TOOLCHAIN_FILE=build/Release/generators/conan_toolchain.cmake -DCMAKE_POLICY_DEFAULT_CMP0091=NEW -DCMAKE_BUILD_TYPE=Release .. && ninja

# Coverage en consola
ctest --rerun-failed --output-on-failure --test-dir tests && gcovr -r .. --filter '../src/' --fail-under-line=90

# Coverage en HTML
# mkdir ../coverage || true && rm -rf ../coverage/* && gcovr -r .. . --filter '../src/' --html --html-details -o ../coverage/coverage_report.html

# Coverage en XML
# mkdir ../coverage || true && rm -rf ../coverage/* && gcovr -r .. . --filter '../src/' --xml -o ../coverage/coverage_report.xml

