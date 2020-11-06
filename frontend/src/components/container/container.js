import styled from "styled-components";

const Wrapper = styled.div`
  width: 100vw;
  height: 100vh;
  overflow: hidden;
  display: flex;
  align-items: center;
  justify-content: center;
`;

const ContainerStyled = styled.div`
  display: block;
  width: 480px;
  height: 800px;
  background: #07202c;
  border: 1px solid #44d194;
  border-radius: 6px;
  box-shadow: 0px 4px 8px rgba(0, 0, 0, 0.14), 0px 4px 16px rgba(0, 0, 0, 0.13),
    0px 4px 24px rgba(0, 0, 0, 0.13);
  padding: 16px;
`;

const Container = ({ children }) => {
  return (
    <Wrapper>
      <ContainerStyled>{children}</ContainerStyled>
    </Wrapper>
  );
};

export default Container;
