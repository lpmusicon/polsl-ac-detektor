import ax from "axios";

export const axios = ax.create({
  baseURL: "http://192.168.4.1",
});

// export const axios = ax;

export { useFetch } from "./use.fetch";
export { usePost } from "./use.post";
export { useDelete } from "./use.delete";
