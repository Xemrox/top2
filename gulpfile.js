const gulp = require('gulp');
const spawn = require('child_process').spawn; //Sync;
const exec = require('child_process').exec;
const fs = require('fs');
const chalk = require('chalk');

const topFolder = "toptest/src/ok";
const pipe = ['pipe','pipe','pipe'];

const Params = ['--err=err.txt','--o=out.txt', '--l=LEX.txt', '--a=AST.txt', '--s=SYM.txt', '--p=PRS.txt'];
const MakeParams = [];
const LBParams = [];

const argv = process.argv.slice(3).map((v) => {
	if(v.startsWith('--')) {
		return v;
	} else if(v.startsWith('-')) {
		return v.slice(1);
	}
	return v;
});

const topFiles = fs.readdirSync(topFolder + '/.');

function spawnMake(args) {
	return spawn('make', MakeParams.concat(argv), {stdio: pipe, cwd: process.cwd()+'/flex-bison/'});
}

function spawnLB(target, redirect) {
	return spawn('flex-bison/top', LBParams.concat([topFolder + '/' + target + '/'+target+'.top'].concat(argv)), {stdio: redirect})
}

var WARN = 0;
function colorOutput(task, taskName, cb) {

	task.stderr.setEncoding('utf-8');
	WARN = 0;
	var wMatch = /.*(\d+) (warnings|warning) generated.*/g;
	task.stderr.on('data', (error) => {
		var match = wMatch.exec(error);
		if(match) {
			//console.error(match[0]);
			console.log(match[1]);
			WARN = WARN + parseInt(match[1]);
		}
		console.error(chalk.bold.red(error.trim()));
	});
	task.stdout.setEncoding('utf-8');
	
	task.stdout.on('data', (data) => {
		console.log(chalk.green(data.trim()));
	});
	task.on('error', (error) => {
		console.error(chalk.bold.red('[E] ' + toString(error)));
	});

	task.on('exit', (code) => {
		const cl = code !== 0 ? chalk.red : chalk.green;
		console.log(chalk.yellow(taskName) + ' exited with code ' + cl(code));
		console.error('got ' + WARN + ' warnings. ');
		cb();
	});
}

topFiles.forEach( (taskName) => {
	taskName = taskName;
	gulp.task(taskName, [], function (cb) {
		const tCase = spawnLB(taskName, pipe);

		colorOutput(tCase, taskName, cb);
	});
	//console.log("created task: " + taskName);
});

gulp.task('all', [], function (cb) {
	var waits = [];
	var results = [];
	topFiles.forEach( (taskName) => {
		var wait = new Promise((resolve, reject) => {
			const tCase = spawnLB(taskName, 'ignore'); //discard output
			tCase.on('error', (error) => {
				console.error(chalk.bold.red(error));
			});
			tCase.on('exit', (code) => {
				const cl = code ? chalk.red : chalk.green;
				console.log(chalk.yellow(taskName) + ' exited with code ' + cl(code));
				resolve({ task: taskName, code: code });
			});

			tCase.on('error', (error) => {
				console.error(error);
			});
		});
		wait.then((value) => {
			results.push(value);
		});
		waits.push(wait);
	});
	Promise.all(waits).then(
		() => {
				var fail = results.filter((v) => {
					return v.code != 0;
				});
				var ok = results.filter((v) => {
					return v.code == 0;
				});
				var sum = fail.length + ok.length;
				console.log(chalk.green("-----------------------"));
				const result = Math.round(ok.length / sum * 100 * 100) / 100;
				const cl = result > 50 ? chalk.green : chalk.red;
				console.log(ok.length + chalk.yellow('/') + sum + " succeeded -> " + cl(result+'%') );
				console.log(chalk.green("-----------------------"));
				cb();
			}
		);
});

gulp.task('make', [], function(cb){
	const make = spawnMake();

	colorOutput(make, 'make', cb)
});

gulp.task('vagrant', [], function(cb) {
	const vagrant = spawnVagrant();

	vagrant.on('error', (err) => {
		console.error(chalk.bold.red(error));
	});

	vagrant.on('exit', (code) => {
		console.log(`Vagrant ssh ended with ${code}`);
		cb();
	});

});

gulp.task('default', [], function () {
	console.log("run either the all task or one of: ");
	const vNames = Object.keys(gulp.tasks).filter((v,k) => {
		return v;
	}).sort();
	console.log(vNames);
});
