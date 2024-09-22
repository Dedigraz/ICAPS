import type { LatLngTuple } from "leaflet";

export type Anomaly = {
	lat: number;
	lng: number;
	offset: number;
	width: number;
	height: number;
	Anomaly:
		| "POTHOLE"
		| "CRACK"
		| "SPEEDBUMP"
		| "COLLISION"
		| "ACCIDENT_SCENE"
		| "MANHOLE"
		| "FIRE_OR_COMBUSTION_ZONE"
		| "VIBRATIONS"
		| string;
};

export interface CarMapProps {
	carLocation: LatLngTuple;
	anomalies: Anomaly[];
	route: LatLngTuple[];
	routeHandler: (startPos: LatLngTuple, endPos:LatLngTuple) => void;
}
