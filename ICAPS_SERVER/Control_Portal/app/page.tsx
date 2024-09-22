"use client";

import { useState, useEffect } from "react";
import io from "socket.io-client";
import dynamic from "next/dynamic";
import axios from "axios";
import type { LatLngTuple } from "leaflet";
import type { Anomaly } from "@/utils/types";
const CarMap = dynamic(() => import("@/components/CarMap"), { ssr: false });

const API_URL = process.env.NODE_ENV === 'production'
  ? process.env.NEXT_PUBLIC_PROD_API_URL
  : process.env.NEXT_PUBLIC_API_URL;

const socket = io(API_URL,{
	transports: ['websocket', 'polling', 'webtransport'],

});
export default function Home() {
	const [carStatus, setCarStatus] = useState({
		status: "Stopped",
		routeCompletion: 0,
		roadHealth: "Healthy",
		speed: 0,
		connectionStatus: "Disconnected",
	});
	const [carLocation, setCarLocation] = useState<LatLngTuple>([
		9.53742, 6.45959,
	]);
	const [anomalies, setAnomalies] = useState<Anomaly[]>([]);
	const [route, setRoute] = useState<LatLngTuple[]>([]);
	const [isStarted, setIsStarted] = useState<boolean>(false);
	useEffect(() => {
		
		fetchData();
		const interval = setInterval(fetchData, 20_000);
		return () => clearInterval(interval);
	}, []);

	const fetchData = async () => {
		try {
			const [statusResponse, locationResponse, anomaliesResponse] =
				await Promise.all([
				axios.get(`${API_URL}/status`),
				axios.get(`${API_URL}/location`),
					axios.get(`${API_URL}/anomalies`),
			]);

			setCarStatus(statusResponse.data);
			setCarLocation(locationResponse.data);
			setAnomalies(anomaliesResponse.data);
		} catch (error) {
			console.error("Error fetching data:", error);
		}
	};
	
	const handleStartStop = async () => {
		try {
			const action = isStarted ? "stop" : "start";
			await axios.post(`${API_URL}/${action}`);
			fetchData();
			setIsStarted(!isStarted);
		} catch (error) {
			console.error("Error starting/stopping:", error);
		}
	};
	const handleRoute = (startPos: LatLngTuple, endPos: LatLngTuple) => {
		setRoute([startPos, endPos]);
	}
	return (
		<main className="flex h-auto grow w-screen relative">
			<CarMap carLocation={carLocation} anomalies={anomalies} route={route} routeHandler={handleRoute}/>
			<div
				className="absolute w-1/4 h-1/4 rounded-t-md bottom-0 left-1/2 bg-white border-2 border-black border-b-0"
				style={{ zIndex: 1000000, transform: "translate(-50%)" }}
			>
				<span className="pl-4 pt-2 font-bold text-xl">Car Status</span>
				<div className="flex flex-col items-left justify-center w-full grow px-4 pt-4">
					<div className="w-full flex flex-row justify-between border-y-2 text-xl">
						<span>Status</span>
						<span>{carStatus.status}</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Route completion</span>
						<span>{carStatus.routeCompletion}%</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Incoming Road Health</span>
						<span>{carStatus.roadHealth}</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Speed</span>
						<span>{carStatus.speed} km/h</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Connection status</span>
						<span>{carStatus.connectionStatus}</span>
					</div>
					<button
						type="button"
						className="mt-4 bg-blue-500 hover:bg-blue-700 text-white font-bold py-2 px-4 rounded"
						onClick={handleStartStop}
					>
						{isStarted ? "Stop" : "Start"}
					</button>
				</div>
			</div>
		</main>
	);
}
