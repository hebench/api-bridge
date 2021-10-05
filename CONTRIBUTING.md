# Contributing

All HEBench repos welcome pull requests from external contributors. In general, HEBench repositories require 3 steps for proper contribution:

1. [Code Formatting](#formatting)
2. [Building & Running](#building-and-running)
3. [Creating a Pull Request](#pull-request-template)

See the [Installing Resources](#resources) section for details on installing/setting up all of the requirements below.

## Code Formatting <a name="formatting"></a>

### Requirements
- clang-format (tested with clang-format-9) 
- pre-commit (tested with version 1.15.2) 

### Steps to Follow
To contribute, please, install **clang-format** on your dev system and **pre-commit** tool and hooks on your local repo clone. This will ensure adherence to code formatting and enforce license text on all C++ source files.

Use the following commands at the root of the project to check formatting prior to committing (assuming all requirements are installed):
```bash
pre-commit install
pre-commit run --all-files
git status # Check if any files were changed by pre-commit
git add .  # assuming files were changed
git commit # assuming files were changed
```

## Building & Running <a name="building-and-running"></a>

### Requirements
- CMake 3.12+
- C++14 capable compiler (tested with GCC version 9.3)
- GLIBC (tested with ldd version 2.31)

### Steps to Follow
Each repository in HEBench has build and run steps specific to the particular component. Please refer to [Building](https://github.com/hebench/api-bridge#building) for the steps specific to the HEBench API Bridge.

## Creating a Pull Request <a name="pull-request-template"></a>

### Requirements
- Git 2.74+

### Steps to Follow
All Pull Requests must follow the the provided [HEBench PR Template](https://github.com/hebench/api-bridge/blob/main/.github/PULL_REQUEST_TEMPLATE.md). When you go to create a [New Pull Request](https://github.com/hebench/api-bridge/compare), the template will be populated in the PR message. Please fill in the empty fields as required.

## Installing Resources <a name="resources"></a>

### clang-format-9
Ubuntu 16.04:
```bash
sudo bash
wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-9 main"
apt-get update
apt-get install clang-format-9
exit
```

Ubuntu 18.04/20.04:
```bash
sudo apt-get install clang-format-9
```

### pre-commit: [https://pre-commit.com](https://pre-commit.com/)
*Note that we use a python virtual environment to avoid potentially touching system python components*

Ubuntu 18.04/20.04:
```bash
sudo apt-get install python3-pip
python3 -m venv VIRTUAL_ENV_DIR
source VIRTUAL_ENV_DIR/bin/activate # This command must be run whenever using pre-commit
echo "appdirs==1.4.4
cfgv==3.2.0
distlib==0.3.1
filelock==3.0.12
identify==2.2.4
nodeenv==1.6.0
pre-commit==2.12.1
PyYAML==5.4.1
six==1.16.0
toml==0.10.2
virtualenv==20.4.6" > requirements.txt
python3 -m pip install -r requirements.txt
deactivate
```

### CMake
```bash
VERSION=3.16.3 # Use any version 3.12+
wget https://github.com/Kitware/CMake/releases/download/v$VERSION/cmake-$VERSION-linux-x86_64.sh
chmod +x cmake-$VERSION-linux-x86_64.sh
./cmake-$VERSION-linux-x86_64.sh
```

### GCC
```bash
sudo apt install build-essential
sudo apt -y install gcc-9 g++-9
```
Ubuntu 18.04:
```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 900 --slave /usr/bin/g++ g++ /usr/bin/g++-9
```
Ubuntu 20.04:
```bash
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9 
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 9
```

### Git
```bash
sudo apt-get install git
```

