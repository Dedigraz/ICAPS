import http from 'k6/http';
import ws from 'k6/ws';
import { check, sleep } from 'k6';
import { randomString, randomIntBetween } from 'https://jslib.k6.io/k6-utils/1.2.0/index.js';
export const options = {
  // A number specifying the number of VUs to run concurrently.
  //vus: 10,
  //iterations: 20,
  // A string specifying the total duration of the test run.
  // duration: '1m',

  // The following section contains configuration options for execution of this
  // test script in Grafana Cloud.
  //
  // See https://grafana.com/docs/grafana-cloud/k6/get-started/run-cloud-tests-from-the-cli/
  // to learn about authoring and running k6 test scripts in Grafana k6 Cloud.
  //
  // cloud: {
  //   // The ID of the project to which the test is assigned in the k6 Cloud UI.
  //   // By default tests are executed in default project.
  //   projectID: "",
  //   // The name of the test in the k6 Cloud UI.
  //   // Test runs with the same name will be grouped.
  //   name: "script.js"
  // },

  // Uncomment this section to enable the use of Browser API in your tests.
  //
  // See https://grafana.com/docs/k6/latest/using-k6-browser/running-browser-tests/ to learn more
  // about using Browser API in your test scripts.
  //
  scenarios: {
    constant_request_rate: {
      executor: 'constant-arrival-rate',
      rate: 1000,
      timeUnit: '1s',
      duration: '13m',
      preAllocatedVUs: 200,
      maxVUs: 200,
    }
    //   // The scenario name appears in the result summary, tags, and so on.
    //   // You can give the scenario any name, as long as each name in the script is unique.
    //   ui: {
    //     // Executor is a mandatory parameter for browser-based tests.
    //     // Shared iterations in this case tells k6 to reuse VUs to execute iterations.
    //     //
    //     // See https://grafana.com/docs/k6/latest/using-k6/scenarios/executors/ for other executor types.
    //     executor: 'shared-iterations',
    //     options: {
    //       browser: {
    //         // This is a mandatory parameter that instructs k6 to launch and
    //         // connect to a chromium-based browser, and use it to run UI-based
    //         // tests.
    //         type: 'chromium',
    //       },
    //     },
    //   },
  }
};

// The function that defines VU logic.
//
// See https://grafana.com/docs/k6/latest/examples/get-started-with-k6/ to learn more
// about authoring k6 scripts.
//
export default function() {
  //http.get('https://localhost:5001/start');
  //http.get(`https://localhost:5001/navigation/route/?startlat=${randomIntBetween(9, 11)}&startlng=${randomIntBetween(6, 8)}&endlat=${randomIntBetween(9, 11)}&endlng=${randomIntBetween(6, 8)}}`);

  sleep(1);
  const url = `wss://localhost:5001/ws/update`;
  const params = { tags: { my_tag: 'my ws session' } };

  const res = ws.connect(url, params, function(socket) {

    socket.on('open', function open() {
      console.log(`VU ${__VU}: connected`);

      socket.send(JSON.stringify({ event: 'SET_NAME', new_name: `Croc ${__VU}` }));

      socket.setInterval(function timeout() {
        socket.send(JSON.stringify({ userId: `client ${__VU}`, lat: randomIntBetween(9, 10), lng: randomIntBetween(6, 7), maneuver: "test" }));
      }, randomIntBetween(2000, 8000)); // say something every 2 - 8 seconds
    });
    socket.setTimeout(function() {
      console.log(`Closing the socket forcefully 3s after graceful LEAVE`);
      socket.close();
    }, (1000 * 60 * 12));
  });

  check(res, { 'Connected successfully': (r) => r && r.status === 101 });
}
