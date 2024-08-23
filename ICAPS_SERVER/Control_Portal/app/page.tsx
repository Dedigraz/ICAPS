"use client";
import dynamic from "next/dynamic";
import Image from "next/image";
// import MapComp from "@/components/Map/Map"

const CarMap = dynamic(() => import("@/components/CarMap"), { ssr: false });

export default function Home() {
	return (
		<main className="flex h-auto grow w-screen relative">
			<CarMap />
			<div
				className="absolute w-1/4 h-1/4 rounded-t-md bottom-0 left-1/2 bg-white border-2 border-black border-b-0"
				style={{ zIndex: 1000000, transform: "translate(-50%)" }}
			>
				<span className="pl-4 pt-2 font-bold text-xl">Car Status</span>
				<div className="flex flex-col items-left justify-center w-full grow px-4 pt-4">
					<div className="w-full flex flex-row justify-between border-y-2 text-xl">
						<span>Status</span>
						<span>Running</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Route completion</span>
						<span>42%</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Incoming Road Health</span>
						<span>Anomaly</span>{/*Anomaly|Healthy*/}
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Speed</span>
						<span>13km/h</span>
					</div>
					<div className="w-full flex flex-row justify-between">
						<span>Connection status</span>
						<span>Connected</span>
					</div>
					
				</div>
			</div>
		</main>
	);
}
