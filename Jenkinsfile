pipeline {
    agent any

    stages {
        stage('Clean before build') {
            steps {
                sh 'rm -rf .[^.] .??* *'
            }
        }
        stage('Checkout') {
            steps {
                checkout([$class: 'GitSCM', branches: [[name: '*/master']], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'SubmoduleOption', disableSubmodules: false, recursiveSubmodules: true, reference: '', trackingSubmodules: false]], submoduleCfg: [], userRemoteConfigs: [[url: 'https://github.com/muhkuh-sys/org.muhkuh.lua-lua51-romloader.git']]])
            }
        }
        stage('Windows') {
            steps {
                sh './.lxc_build_windows.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 14.04 x64') {
            steps {
                sh './.lxc_build_ubuntu_1404_x64.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 14.04 x86') {
            steps {
                sh './.lxc_build_ubuntu_1404_x86.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 16.04 x64') {
            steps {
                sh './.lxc_build_ubuntu_1604_x64.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 16.04 x86') {
            steps {
                sh './.lxc_build_ubuntu_1604_x86.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 16.10 x64') {
            steps {
                sh './.lxc_build_ubuntu_1610_x64.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 16.10 x86') {
            steps {
                sh './.lxc_build_ubuntu_1610_x86.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 17.04 x64') {
            steps {
                sh './.lxc_build_ubuntu_1704_x64.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Ubuntu 17.04 x86') {
            steps {
                sh './.lxc_build_ubuntu_1704_x86.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Build Artefacts') {
            steps {
                sh './.lxc_build_artefacts.sh'
            }
            post {
                failure {
                    /* Stop and remove the running container. Do not fail on this commands. */
                    sh 'lxc stop c0 || true'
                    sh 'lxc delete c0 || true'
                }
            }
        }
        stage('Save Artifacts') {
            steps {
                archive 'build/*.xml,build/*.tar.xz,build/*.hash,build/*.pom'
            }
        }
        stage('Clean after build') {
            steps {
                sh 'rm -rf .[^.] .??* *'
            }
        }
    }
}
