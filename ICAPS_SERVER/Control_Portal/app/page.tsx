"use client";
import dynamic from "next/dynamic";
import Image from "next/image";
// import MapComp from "@/components/Map/Map"

const CarMap = dynamic(() => import("@/components/CarMap"), { ssr: false });

export default function Home() {
	return (
		<main className="flex h-auto grow w-screen">	
			<CarMap />
		</main>
	);
}
