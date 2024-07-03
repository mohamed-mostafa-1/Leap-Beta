// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/10.12.0/firebase-app.js";
import {
  getDatabase,
  ref,
  onValue,
  set,
  update
} from "https://www.gstatic.com/firebasejs/10.12.0/firebase-database.js";

const firebaseConfig = {

  apiKey: "AIzaSyCGmKD7DbhuzGrDXoh_g5CrI_gFCk8d2IQ",

  authDomain: "leapcariotproject.firebaseapp.com",

  databaseURL: "https://leapcariotproject-default-rtdb.firebaseio.com",

  projectId: "leapcariotproject",

  storageBucket: "leapcariotproject.appspot.com",

  messagingSenderId: "434998406345",

  appId: "1:434998406345:web:0360665b2bf7f249d37118"

};


const app = initializeApp(firebaseConfig);
const database = getDatabase();
const rootRef = ref(database);

let data;
// To fetch the data from Firebase
function fetchData(callback) {
	onValue(
  	rootRef,
  	(snapshot) => {
    	data = snapshot.val();
	callback(data);
	  },
	  {
	    onlyOnce: false
	  }
	);
}

const directionRef = ref(database, "Directions");
const speedRef = ref(database, "Speed");

export { fetchData, directionRef, speedRef, update };
