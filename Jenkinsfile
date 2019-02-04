def tascar_build_steps(stage_name) {
    // Extract components from stage_name:
    def system, arch, devenv
    (system,arch,devenv) = stage_name.split(/ *&& */) // regexp for missing/extra spaces

    // checkout tascarpro from version control system, the exact same revision that
    // triggered this job on each build slave
    checkout scm

    // Avoid that artifacts from previous builds influence this build
    sh "git reset --hard && git clean -ffdx"

    // Autodetect libs/compiler
    sh "make pack"

    // Store debian packets for later retrieval by the repository manager
    stash name: (arch+"_"+system), includes: 'packaging/pack/'
}

pipeline {
    agent {label "jenkinsmaster"}
    stages {
        stage("build") {
            parallel {
		stage(                        "bionic && x86_64 && tascardev") {
                    agent {label              "bionic && x86_64 && tascardev"}
                    steps {tascar_build_steps("bionic && x86_64 && tascardev")}
                }
                stage(                        "xenial && x86_64 && tascardev") {
                    agent {label              "xenial && x86_64 && tascardev"}
                    steps {tascar_build_steps("xenial && x86_64 && tascardev")}
                }
                stage(                        "trusty && x86_64 && tascardev") {
                    agent {label              "trusty && x86_64 && tascardev"}
                    steps {tascar_build_steps("trusty && x86_64 && tascardev")}
                }	
	    }
	}
	stage("preparepublish") {
	    agent {label "aptly"}
	    // do not publish packages for any branches except these
	    when { anyOf { branch 'master'; branch 'development' } }
	    steps {
                // receive all deb packages from tascarpro build
                unstash "x86_64_bionic"
                unstash "x86_64_xenial"
                unstash "x86_64_trusty"

                // Copies the new debs to the stash of existing debs,
                // creates an apt repository, uploads.
                sh "make -C aptly"

	    }
	}
        stage("publish") {
	    agent {label "aptly"}
	    // do not publish packages for any branches except these
	    when { anyOf { branch 'master'; branch 'development' } }
	    steps {
                checkout([$class: 'GitSCM', branches: [[name: "$BRANCH_NAME"]], doGenerateSubmoduleConfigurations: false, extensions: [[$class: 'CleanCheckout']], submoduleCfg: [], userRemoteConfigs: [[url: "ssh://git@mha.physik.uni-oldenburg.de/hoertech-aptly"]]])

                // Copies the new debs to the stash of existing debs,
                // creates an apt repository, uploads.
                sh "make"

	    }
        }
    }
    post {
        failure {
	    mail to: 'g.grimm@hoertech.de',
	    subject: "Failed Pipeline: ${currentBuild.fullDisplayName}",
	    body: "Something is wrong with ${env.BUILD_URL} ($GIT_URL)"
        }
    }
}
